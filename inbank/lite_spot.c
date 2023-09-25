/* Lights up given location				-RAK-	*/
//void __fastcall__ lite_spot(int8u y, int8u x)
{
  //if (dungeon1_panel_contains(y, x))
    //print(loc_symbol(y, x), y, x);

  if ((y >= panel_row_min) && (y <= panel_row_max) && (x >= panel_col_min) && (x <= panel_col_max))		// in-line panel_contains code  -SJ
    bank_print_symbol(y, x, INBANK_RETBANK);
}