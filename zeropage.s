
			.export _old_vbxe_bank, _vbxe_save_bank, _vbxe_scrnptr
			.export _char_row, _char_col
			.export _tcptr, _mfptr, _mon_tot_mult
			.export _cur_height, _cur_width
			.export _max_panel_rows, _max_panel_cols, _panel_row, _panel_col
			.export _panel_row_min, _panel_row_max, _panel_col_min, _panel_col_max
			.export _player_light, _dun_level
			.export _doorindex, _tunindex, _wallindex
			.export _btzp_y, _btzp_x, _btzp_start_row, _btzp_start_col
			.export _btzp_tun_ptr, _btzp_tmp_row, _btzp_tmp_col
			.export _btzp_i, _btzp_j, _btzp_c_ptr, _btzp_d_ptr
			.export _cgzp_y1, _cgzp_x1, _cgzp_y2, _cgzp_x2, _cgzp_pick1, _cgzp_pick2
			.export _pszp_t1, _pszp_t2
			.export _los_dx, _los_dy, _los_sx, _los_sy, _los_err, _los_e2


			.zeropage
			
_old_vbxe_bank:		.res 1		; from vbxebank.c
_vbxe_save_bank:	.res 1		; from vbxetext.c
_vbxe_scrnptr:		.res 2		; from vbxetext.c
	
_char_row:      	.res 1          ; from player.c
_char_col:      	.res 1          ; from player.c
_tcptr:			.res 1		; from treasure.c - was .res 2, max is 175 -SJ
_mfptr:			.res 1		; from monsters.c - was .res 2, max is 125 -SJ
_mon_tot_mult:		.res 1		; from monsters.c

_cur_height:		.res 1		; from variable.c
_cur_width:		.res 1		; from variable.c
_max_panel_rows:	.res 1		; from variable.c
_max_panel_cols:	.res 1		; from variable.c
_panel_row:		.res 1		; from variable.c
_panel_col:		.res 1		; from variable.c
_panel_row_min:		.res 1		; from variable.c
_panel_row_max:		.res 1		; from variable.c
_panel_col_min:		.res 2		; from variable.c
_panel_col_max:		.res 2		; from variable.c

;int16 panel_col_prt;
;int8 panel_row_prt;
_player_light:		.res 1		; from variable.c
_dun_level:		.res 1		; from variable.c

_doorindex:		.res 1		; from generate1.c
_tunindex:		.res 2		; from generate1.c
_wallindex:		.res 2		; from generate1.c
_btzp_y:		.res 1		; from generate1.c build_tunnel
_btzp_x:		.res 1		; from generate1.c build_tunnel
_btzp_start_row:	.res 1		; from generate1.c build_tunnel
_btzp_start_col:	.res 1		; from generate1.c build_tunnel
_btzp_tun_ptr:		.res 2		; from generate1.c build_tunnel
_btzp_tmp_row:		.res 1		; from generate1.c build_tunnel
_btzp_tmp_col:		.res 1		; from generate1.c build_tunnel
_btzp_i:		.res 1		; from generate1.c build_tunnel
_btzp_j:		.res 1		; from generate1.c build_tunnel
_btzp_c_ptr:		.res 2		; from generate1.c build_tunnel
_btzp_d_ptr:		.res 2		; from generate1.c build_tunnel
_cgzp_y1:		.res 1		; from generate1.c cave_gen
_cgzp_x1:		.res 1		; from generate1.c cave_gen
_cgzp_y2:		.res 1		; from generate1.c cave_gen
_cgzp_x2:		.res 1		; from generate1.c cave_gen
_cgzp_pick1:		.res 1		; from generate1.c cave_gen
_cgzp_pick2:		.res 1		; from generate1.c cave_gen

_pszp_t1:		.res 1		; from caveplace.c place_streamer
_pszp_t2:		.res 1		; from caveplace.c place_streamer

_los_dx:		.res 1		; from inbank\los.c, int8 dx
_los_dy:		.res 1		; from inbank\los.c, int8 dy
_los_sx:		.res 1		; from inbank\los.c, int8 sx
_los_sy:		.res 1		; from inbank\los.c, int8 sy
_los_err:		.res 1		; from inbank\los.c, int8 err
_los_e2:		.res 1		; from inbank\los.c, int8 e2

