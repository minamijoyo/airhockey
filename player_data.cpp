
/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player_data.cpp (�v���C���[�̈ʒu���W�Ǘ��N���X)
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

//���W�v�Z����
void CPlayerData::sum_player_position(int player_number){	
	int height=80;//�e�[�u���̍���cm
	int qu=link_quality_[player_number*2];//��̃A���e�i
	int qb=link_quality_[player_number*2+1];//���̃A���e�i

	//���`�ߎ��̃p�����[�^
	double a =-0.666*input_kando_;//�X��
	double b=158.48*input_kando_;//�ؕ�

	//���W�̌v�Z
	double posu=a*qu+b;//��̃A���e�i
	double posb=height-(a*qb+b);//���̃A���e�i
	double pos=0.5*posu+0.5*posb;//�����̃A���e�i�̕���

	//�e�[�u���̍�����-0.05�`1.05�ɐ��K��
	double normal_pos=(pos/height);
	if(normal_pos>1.05) normal_pos=1.05;
	else if(normal_pos<-0.05) normal_pos=-0.05;

	//�O��̈ʒu��p���ĕ�����
	double smooth_pos=0.5*normal_pos+0.5*last_player_position[player_number];
	//0����100�܂�11�i�K�Ɋۂ�
	int tmp=(int)(smooth_pos*100);
	tmp=tmp+(tmp%10>=5?10:0)-tmp%10;
	player_position[player_number]=tmp/100.0;
	//�O����W�̍X�V
	last_player_position[player_number]=player_position[player_number];
}
