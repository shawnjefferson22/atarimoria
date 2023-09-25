/* generates damage for 2d6 style dice rolls */
//int16u __fastcall__ damroll(int8u num, int8u sides)
{
  int8u i;
  int16u sum = 0;


  for (i = 0; i < num; ++i)
    sum += randint(sides);
  return(sum);
}