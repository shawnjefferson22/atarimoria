/* Given direction "dir", returns new row, column location -RAK- */
//int8u __fastcall__ mmove(int8u dir, int8u *y, int8u *x)
{
  int8 new_row, new_col;
  int8u bool;


  switch(dir) {
    case 1:
      new_row = *y + 1;
      new_col = *x - 1;
      break;
    case 2:
      new_row = *y + 1;
      new_col = *x;
      break;
    case 3:
      new_row = *y + 1;
      new_col = *x + 1;
      break;
    case 4:
      new_row = *y;
      new_col = *x - 1;
      break;
    case 5:
      new_row = *y;
      new_col = *x;
      break;
    case 6:
      new_row = *y;
      new_col = *x + 1;
      break;
    case 7:
      new_row = *y - 1;
      new_col = *x - 1;
      break;
    case 8:
      new_row = *y - 1;
      new_col = *x;
      break;
    case 9:
      new_row = *y - 1;
      new_col = *x + 1;
      break;
  }

  bool = FALSE;
  if ((new_row >= 0) && (new_row < cur_height) && (new_col >= 0) && (new_col < cur_width)) {
    *y = new_row;
    *x = new_col;
    bool = TRUE;
  }

  return(bool);
}
