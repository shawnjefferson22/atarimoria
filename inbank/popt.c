/* Gives pointer to next free space -RAK-   */
//int8u __fastcall__ popt(void)
{
  if (tcptr == MAX_TALLOC)
    bank_compact_objects(INBANK_RETBANK);

  ++tcptr;					// increase tcptr, next available spot -SJ
  return(tcptr - 1);		// return space we can use -SJ
}
