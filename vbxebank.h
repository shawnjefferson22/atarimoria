#ifndef VBXEBANK_H
#define VBXEBANK_H


#define VBXE_OBJBANK    0x88		// bank 0x08
#define VBXE_MONBANK    0x90		// bank 0x10
#define VBXE_SPLBANK    0x98		// bank 0x18
#define VBXE_MAGBANK    0xA0		// bank 0x20
#define VBXE_CAVBANK    0xA8		// bank 0x28
#define VBXE_GENBANK	0xB0		// bank 0x30


// OBJBANK:	object_list, sorted_objects, t_level, t_list, inventory
//
// MONBANK: c_list, monster_attacks, m_list, m_level, blank_monster, mon_tot_mult
//
// SPLBANK: store, rgold_adj, store_choice, owners
//			player_hp, *player_title, player_exp, race, background, class, class_level_adj, blows_table
// 			spell_learned, spell_worked, spell_forgotten, spell_order, magic_spell, *spell_names,
//
// MAGBANK: *colors, *mushrooms, *woods, *metals, *rocks, *amulets, *syllables, *special_names
//			titles, normal_table,
//
// CAVBANK:	cave, new_level_flag, teleport_flag, player_light, eof_flag, light_flag
//
// GENBANK: scratch ram for cave generation, monster_recall
//



extern unsigned char old_vbxe_bank;
#pragma zpsym("old_vbxe_bank")


void __fastcall__ vbxe_bank(unsigned char bank);
void vbxe_restore_bank(void);


#endif