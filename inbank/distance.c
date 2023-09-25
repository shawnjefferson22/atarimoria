/* Distance between two points                          -RAK-   */
//int8u __fastcall__ distance(int8u y1, int8u x1, int8u y2, int8u x2)
{
  register int8 dy, dx;


  dy = y1 - y2;
  if (dy < 0)
    dy = -dy;
  dx = x1 - x2;
  if (dx < 0)
    dx = -dx;

  return ((((dy + dx) << 1) - (dy > dx ? dx : dy)) >> 1);
}