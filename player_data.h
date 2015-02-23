#ifndef _PLAYER_DATA_H
#define _PLAYER_DATA_H

/**********************************************************
//
//		Wih (Wireless Interface Hockey)
//
//		player_data.h (プレイヤーの位置座標管理クラス)
//
//		Copyright (C) 2007,
//		Masayuki Morita, Kazuki Hamada, Tatsuya Mori,
//		All Rights Reserved.
**********************************************************/

class CPlayerData{
private:
	int link_quality_[4];
	double player_position[2];
	double last_player_position[2];
	bool flag[4];
	double input_kando_;

	void sum_player_position(int player_number);//座標計算部分
public:
	CPlayerData();
	void update_link_quality(int link_number,int link_data);
	double get_player_position(int player_number){return player_position[player_number];};
};

#endif