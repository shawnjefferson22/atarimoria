/* Checks a co-ordinate for in bounds status            -RAK-   */
//int8u __fastcall__ in_bounds(int8u y, int8u x)
{
  if ((y > 0) && (y < cur_height-1) && (x > 0) && (x < cur_width-1))
    return(TRUE);
  else
    return(FALSE);
}
