/* Returns a pointer to next free space	-RAK-	 */
/* Returns 255 if could not allocate a monster.  */
//int8u __fastcall__ popm(void)
{
  if (mfptr == MAX_MALLOC) {					// maxmalloc is 125 so can use int8 here! -SJ
    if (!bank_compact_monsters(INBANK_RETBANK))
	  return 255;
  }

  ++mfptr;
  return(mfptr-1);
}
