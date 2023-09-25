/* Tests a given point to see if it is within the screen -RAK-	*/
/* boundaries.							  */
//int8u __fastcall__ panel_contains(int8u y, int8u x)
{
  if ((y >= panel_row_min) && (y <= panel_row_max) &&
      (x >= panel_col_min) && (x <= panel_col_max))
    return (TRUE);
  else
    return (FALSE);
}
