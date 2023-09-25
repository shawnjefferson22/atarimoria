/* Adjustment for wisdom/intelligence				-JWT-	*/
//int8u __fastcall__ stat_adj(int8u stat)
{
   int8u value;


   value = py.stats.use_stat[stat];
   if (value > 117)
     return(7);
   else if (value > 107)
     return(6);
   else if (value > 87)
     return(5);
   else if (value > 67)
     return(4);
   else if (value > 17)
     return(3);
   else if (value > 14)
     return(2);
   else if (value > 7)
     return(1);
   else
     return(0);
}