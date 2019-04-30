#include "sim_foundation.h"
#include "mess_queue.h"
#include "SStd.h"
#include <string>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

//***************************************************************************//
//***************************************************************************//
sim_foundation * sim_foundation::s_f_ = 0;
void init_file(ifstream & inFile_);
//***************************************************************************//
sim_foundation::sim_foundation(): 
	inter_network_(),
	ary_size_(0),
	cube_size_(0),
	router_counter_(0),
	packet_counter_(0),
	inFile_()

{
	s_f_ = this;
	ary_size_ = configuration::ap().ary_number();
	cube_size_ = configuration::ap().cube_number();
	long vc_size = configuration::ap().virtual_channel_number();
	long buff_size = configuration::ap().buffer_size();
	long outbuff_size = configuration::ap().outbuffer_size();
	long flit_size = configuration::ap().flit_size();

	// + 1 means, one for injection
	long phy_ports_t = cube_size_ * 2 + 1;
	router_counter_ = ary_size_;
	for(long i = 0; i < cube_size_ - 1; i++) {
		router_counter_ = router_counter_ * ary_size_;
	}
	add_type add_t;
	add_t.resize(cube_size_, 0);
	for(long i = 0; i < router_counter_; i++) {
		inter_network_.push_back(sim_router_template
			(phy_ports_t, vc_size, buff_size, outbuff_size, add_t, 
			 ary_size_, flit_size)); 
		//assign the address of the router
		add_t[cube_size_ - 1]++;
		for(long j = cube_size_ -1; j > 0; j--) {
			if(add_t[j] == ary_size_) {
				add_t[j] = 0;
				add_t[j-1]++;
			}
		}
			
	}
	init_file();
}
void sim_foundation::probability_init(){
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			HT_probability[i][j] = 0.01;
		}
	}
}
//***************************************************************************//
void sim_foundation::init_file()
{
	inFile_.open(configuration::wap().trace_fname().c_str());
    if(!inFile_) {
       cerr<<"Can not open source file."<<endl;
       assert(0);
    }
    time_type event_time_t;
	inFile_ >> event_time_t;
	mess_queue::wm_pointer().add_message(mess_event(
						event_time_t, EVG_));
	if(detection_open){
		srand((unsigned)time(0));
		mess_queue::wm_pointer().add_message(mess_event(event_time_t, DETECT_));
		sim_foundation::probability_init();
		mess_queue::wm_pointer().add_message(mess_event(1000, CALCULATE_));
	}
}

//***************************************************************************//
ostream& operator<<(ostream& os, const sim_foundation& sf)
{
	os <<"************Router list*************"<<endl;
	vector<sim_router_template>::const_iterator first = (sf.inter_network_)
																	.begin();
	vector<sim_router_template>::const_iterator last = (sf.inter_network_)
																	  .end();
	for(; first != last; first++) {
		os<<(*first);
	}
	return os;
}

//***************************************************************************//
sim_router_template & sim_foundation::router(const add_type & a) 
{
	add_type::const_iterator first = a.begin();
	add_type::const_iterator last = a.end();
	long i = (* first); first++;
	for(; first!= last; first++) {
		i = i * ary_size_ + (*first);
	}
	return (inter_network_[i]);
}

//***************************************************************************//
const sim_router_template & sim_foundation::router(const 
			add_type & a) const 
{
	add_type::const_iterator first = a.begin();
	add_type::const_iterator last = a.end();
	long i = (* first); first ++;
	for(; first!= last; first++) {
		i = i * ary_size_ + (*first);
	}
	return (inter_network_[i]);
}

//***************************************************************************//
void sim_foundation::receive_EVG_message(mess_event mesg)
{
	//first, inject the flits 
    add_type sor_addr_t;
    add_type des_addr_t;
    long pack_size_t;
    for(long i = 0; i < cube_size_; i++) {
		long t; inFile_ >> t;
		Sassert(!inFile_.eof());
		sor_addr_t.push_back(t);
	}
    for(long i = 0; i < cube_size_; i++) {
		long t; inFile_ >> t;
		Sassert(!inFile_.eof());
		des_addr_t.push_back(t);
	}
	inFile_ >> pack_size_t;
	Sassert(!inFile_.eof());
	router(sor_addr_t).receive_packet();
	packet_counter_++;

    if(!inFile_.eof()) {
        time_type event_time_t;
		inFile_ >> event_time_t;
		if(!inFile_.eof()) {
			mess_queue::wm_pointer().add_message(mess_event(
						event_time_t, EVG_));
		}
	}

}
//***************************************************************************//
//evaluate the address
bool sim_foundation::valid_address(const add_type & a) const {
	if(a.size() != cube_size_) {
		return false;
	}
	for(long i = 0; i < a.size(); i++) {
		if((a[i] >= ary_size_) || (a[i] < 0)) {
			return false;
		}
	}
	return true;
}

//***************************************************************************//
//router pipeline stage
void sim_foundation::receive_ROUTER_message(mess_event mesg)
{
	mess_queue::wm_pointer().add_message(mess_event(
				mesg.event_start() + PIPE_DELAY_, ROUTER_));

	for(long i = 0; i < router_counter_; i++) {
		inter_network_[i].router_sim_pwr();
	}
}

//***************************************************************************//
void sim_foundation::receive_WIRE_message(mess_event mesg)
{
	add_type des_t = mesg.des();
	long pc_t = mesg.pc();
	long vc_t = mesg.vc();
	flit_template & flits_t = mesg.get_flit();
	router(des_t).receive_flit(pc_t, vc_t, flits_t);
}

//***************************************************************************//
void sim_foundation::receive_CREDIT_message(mess_event mesg)
{
	add_type des_t = mesg.des();
	long pc_t = mesg.pc();
	long vc_t = mesg.vc();
	router(des_t).receive_credit(pc_t, vc_t);
}

//***************************************************************************//
//detection message 
void sim_foundation::receive_DETECT_message(mess_event mesg)
{
	add_type des_t;
	des_t.push_back(rand()%9);
	des_t.push_back(rand()%9);
	add_type src_t;
	src_t.push_back(rand()%9);
	src_t.push_back(rand()%9);
	long pack_size_t = 3;
	time_type time_t = mesg.event_start()+100;
	router(src_t).receive_detect_packet(src_t,des_t,time_t,pack_size_t);
	record_type rec;
	rec.des = des_t;
	rec.src = src_t;
	rec.res = 0;
	detection_record.push_back(rec);
	time_t += 5;
	mess_queue::wm_pointer().add_message(mess_event(time_t, DETECT_));
}

void sim_foundation::receive_CALCULATE_message(mess_event mesg)
{
	//calculation
	vector<add_type> nodes;
	long dx,dy;
	for(int i=0;i<detection_record.size();i++){
		//path's nodes and detection result calculation
		nodes = sim_foundation::getNodesByRecord(detection_record[i]);
		//probability calculation and update
		sim_foundation::updateProbability(nodes,detection_record[i].res);

	}

	//probability calculation

	//record and caculation output
	static int cyc = 0;
	ofstream recordFile;
	if(cyc==0){recordFile.open("./record/detection1.txt",ios::out|ios::trunc);} 
	else {recordFile.open("./record/detection1.txt",ios::app);}
	ofstream recordFile2;
	if(cyc==0){recordFile2.open("./record/detection2.txt",ios::out|ios::trunc);} 
	else {recordFile2.open("./record/detection2.txt",ios::app);}
	//record
	recordFile<<"Now cycle time is "<<mesg.event_start()<<endl;
	recordFile<<"calculation cycle "<<cyc++<<":"<<endl;
	for(int i=0;i<detection_record.size();i++){
		recordFile<<detection_record[i].src[0]<<" ";
		recordFile<<detection_record[i].src[1]<<" ";
		recordFile<<detection_record[i].des[0]<<" ";
		recordFile<<detection_record[i].des[1]<<" ";
		recordFile<<detection_record[i].res<<endl;
	}
	detection_record.clear();
	//HT probability
	recordFile<<"HT probability: "<<endl;
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			recordFile<<HT_probability[i][j]<<" ";
		}
		recordFile<<endl;
	}
	//HT-judgement rate and disjudgement rate
	vector<double> detection_effect;
	detection_effect = sim_foundation::getDetectionEffect();
	recordFile2<<cyc<<" ";
	recordFile2<<total_detection<<" ";
	recordFile2<<detection_effect[0]<<" ";
	recordFile2<<detection_effect[1]<<endl;

	recordFile.close();
	recordFile2.close();
	
	time_type time_t = mesg.event_start()+5000;
	mess_queue::wm_pointer().add_message(mess_event(time_t, CALCULATE_));
	

}

vector<add_type> sim_foundation::getNodesByRecord(record_type &record)
{
	vector<add_type> resv;
	add_type res;
	long dx = record.des[0] - record.src[0];
	long fx = dx/abs(dx);
	long dy = record.des[1] - record.src[1];
	long fy = dy/abs(dy);
	for(long i=record.src[0];i!=record.des[0];i+=fx){
		res.push_back(i);
		res.push_back(record.src[1]);
		resv.push_back(res);
		if(infect_router[record.src[1]][i]){
			record.res = 1;
		};
		res.clear();
	}
	res.push_back(record.des[0]);
	res.push_back(record.src[1]);
	resv.push_back(res);
	if(infect_router[record.src[1]][record.des[0]]){
		record.res = 1;
	};
	res.clear();
	for(long i=record.src[1];i!=record.des[1];i+=fy){
		res.push_back(record.des[0]);
		res.push_back(i);
		resv.push_back(res);
		if(infect_router[i][record.des[0]]){
			record.res = 1;
		};
		res.clear();
	}
	res.push_back(record.des[0]);
	res.push_back(record.des[1]);
	resv.push_back(res);
	if(infect_router[record.des[1]][record.des[0]]){
		record.res = 1;
	};
	res.clear();
	return resv;
}

void sim_foundation::updateProbability(vector<add_type> nodes, long res)
{
	double Pb_a,Pb_na;
	if(res == 1){
		Pb_a = 0.7;
		Pb_na = 0.5;
	}else{
		Pb_a = 0.1;
		Pb_na = 0.8;
	}
	add_type tmp;
	for(int i=0;i<nodes.size();i++){
		tmp = nodes[i];
		HT_probability[tmp[1]][tmp[0]] = HT_probability[tmp[1]][tmp[0]]*Pb_a/(HT_probability[tmp[1]][tmp[0]]*Pb_a+(1.0-HT_probability[tmp[1]][tmp[0]])*Pb_na);
		if(HT_probability[tmp[1]][tmp[0]]<0.00001) HT_probability[tmp[1]][tmp[0]] = 0.00001;
		if(HT_probability[tmp[1]][tmp[0]]>0.9) HT_probability[tmp[1]][tmp[0]] = 0.9;
	}
}

vector<double> sim_foundation::getDetectionEffect()
{
	double HTtol, nortol,judnum,disjudnum;
	double HTjudr,disjudr;
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			if(infect_router[i][j]==1){
				HTtol++;
				if(HT_probability[i][j]>=0.8)judnum++;
			}
			if(infect_router[i][j]==0){
				nortol++;
				if(HT_probability[i][j]>=0.8)disjudnum++;
			}
		}
	}
	HTjudr = judnum/HTtol;
	disjudr = disjudnum/nortol;
	vector<double> res;
	res.push_back(HTjudr);
	res.push_back(disjudr);
	return res;
}
//***************************************************************************//
void sim_foundation::simulation_results()
{
	vector<sim_router_template>::const_iterator first = 
							inter_network_.begin();
	vector<sim_router_template>::const_iterator last = 
							inter_network_.end();
	double total_delay = 0;
	//calculate the total delay
	first = inter_network_.begin();
	for(; first != last; first++) {
		total_delay += first->total_delay();
		//cout<<first->total_delay()<<endl;
	}
	long tot_f_t = mess_queue::wm_pointer().total_finished();

	double total_mem_power = 0;
	double total_crossbar_power = 0;
	double total_arbiter_power = 0;
	double total_power = 0;
	double total_link_power = 0;
	vector<sim_router_template>::iterator ffirst = 
							inter_network_.begin();
	vector<sim_router_template>::iterator llast = 
							inter_network_.end();
	ffirst = inter_network_.begin();
	for(; ffirst != llast; ffirst++) {
		total_mem_power += ffirst->power_buffer_report();
		total_crossbar_power += ffirst->power_crossbar_report();
		total_arbiter_power += ffirst->power_arbiter_report();
		total_link_power += ffirst->power_link_report();
	}
	time_type curr_time = mess_queue::m_pointer().current_time();
	total_mem_power /= curr_time;
	total_crossbar_power /= curr_time;
	total_link_power /= curr_time;
	total_arbiter_power /= curr_time;
	total_power = total_mem_power + total_crossbar_power + total_arbiter_power +
		total_link_power;

	cout.precision(6);
	cout<<"**************************************************"<<endl;
	cout<<"total finished:       "<<tot_f_t<<endl;
	cout<<"average Delay:        "<< total_delay/tot_f_t<<endl;
	cout<<"total mem power:      "<<total_mem_power * POWER_NOM_<<endl;
	cout<<"total crossbar power: "<<total_crossbar_power * POWER_NOM_<<endl;
	cout<<"total arbiter power:  "<<total_arbiter_power * POWER_NOM_<<endl;
	cout<<"total link power:     "<<total_link_power * POWER_NOM_<<endl;
	cout<<"total power:          "<<total_power * POWER_NOM_<<endl;
	cout<<"infect sum:	"<<infect_sum<<endl;
	//cout<<"local average delay:	"<<local_delay[2][2]/local_count[2][2]<<endl;
	//cout<<"others average delay:"<<(total_delay-local_delay[2][2])/(tot_f_t-local_count[2][2])<<endl;
	cout<<"total detection incoming:"<<total_detection<<endl;
	//cout<<"detection record count:"<<detection_record.size()<<endl;
	cout<<"**************************************************"<<endl;
}

//***************************************************************************//
//Check if the network is back to the inital state.
void sim_foundation::simulation_check()
{
	vector<sim_router_template>::const_iterator first = 
					inter_network_.begin();
	vector<sim_router_template>::const_iterator last = 
					inter_network_.end();
	for(; first != last; first++) {
		first->empty_check();
	}
	cout<<"simulation empty check is correct.\n";
}

//***************************************************************************//
string sim_foundation:: file_name_ =
 string("Invaid file name.\n");

//***************************************************************************//
