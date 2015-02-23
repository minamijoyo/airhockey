
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player_data.cpp (プレイヤーの位置座標管理クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/



#include"player_data.h"
#include"def_common.h"
#include<iostream>

extern SGameSetting g_game_setting;

CPlayerData::CPlayerData(){
	for(int i=0;i<4;i++)link_quality_[i]=0;
	for(int i=0;i<2;i++){
		player_position[i]=0.5;
		last_player_position[i]=0.5;
		flag[i]=false;
	}
	input_kando_=g_game_setting.input_kando_;
	
}

void CPlayerData::update_link_quality(int link_number,int link_data){
	link_quality_[link_number]=link_data;
	if(link_number%2){
		if(flag[link_number-1]){
			sum_player_position((int)(link_number/2));
			flag[link_number-1]=false;
			flag[link_number]=false;
		}
		else flag[link_number]=true;
	}
	else{
		if(flag[link_number+1]){
			sum_player_position((int)(link_number/2));
			flag[link_number+1]=false;
			flag[link_number]=false;
		}
		else flag[link_number]=true;
	}
}	

//座標計算部分
void CPlayerData::sum_player_position(int player_number){	
	int height=80;//テーブルの高さcm
	int qu=link_quality_[player_number*2];//上のアンテナ
	int qb=link_quality_[player_number*2+1];//下のアンテナ

	//線形近似のパラメータ
	double a =-0.666*input_kando_;//傾き
	double b=158.48*input_kando_;//切片

	//座標の計算
	double posu=a*qu+b;//上のアンテナ
	double posb=height-(a*qb+b);//下のアンテナ
	double pos=0.5*posu+0.5*posb;//両方のアンテナの平均

	//テーブルの高さで-0.05～1.05に正規化
	double normal_pos=(pos/height);
	if(normal_pos>1.05) normal_pos=1.05;
	else if(normal_pos<-0.05) normal_pos=-0.05;

	//前回の位置を用いて平滑化
	double smooth_pos=0.5*normal_pos+0.5*last_player_position[player_number];
	//0から100まで11段階に丸め
	int tmp=(int)(smooth_pos*100);
	tmp=tmp+(tmp%10>=5?10:0)-tmp%10;
	player_position[player_number]=tmp/100.0;
	//前回座標の更新
	last_player_position[player_number]=player_position[player_number];
}
