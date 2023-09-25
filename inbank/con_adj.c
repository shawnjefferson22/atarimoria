/* Returns a character's adjustment to hit points	 -JWT-	 */
//int8 __fastcall__ con_adj(void)
{
  int8u con;


  con = py.stats.use_stat[A_CON];
  if (con < 7)
    return(con - 7);
  else if (con < 17)
    return(0);
  else if (con ==  17)
    return(1);
  else if (con <  94)
    return(2);
  else if (con < 117)
    return(3);
  else
    return(4);
}