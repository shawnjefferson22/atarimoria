//void __fastcall__ lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr)
{
  vbxe_bank(VBXE_MONBANK);
  if (!m_ptr->ml)
    strcpy(m_name, "it");
  else
    sprintf(m_name, "the %s", r_ptr->name);

  vbxe_restore_bank();
}
