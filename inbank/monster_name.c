//void __fastcall__ monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
{
  vbxe_bank(VBXE_MONBANK);
  if (!m_ptr->ml)
    strcpy(m_name, "It");
  else
    sprintf(m_name, "The %s", r_ptr->name);

  vbxe_restore_bank();
}
