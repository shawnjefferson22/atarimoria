@ECHO OFF
ca65 -t atari -I c:\cc65\asminc crt0.s
ca65 -t atari -I c:\cc65\asminc copyseg.s
ca65 -t atari -I C:\cc65\asminc irqhand.s
ca65 -t atari -I c:\cc65\asminc vbxe.s
ca65 -t atari -I c:\cc65\asminc zeropage.s
ca65 -t atari -I c:\cc65\asminc cgetc.s

cc65 -t atari -T -Osir bankcode0.c
cc65 -t atari -T -Osir bankcode1.c
cc65 -t atari -T -Osir bankcode2.c
cc65 -t atari -T -Osir cast.c
cc65 -t atari -T -Osir caveplace.c
cc65 -t atari -T -Osir create1.c
cc65 -t atari -T -Osir create2.c
cc65 -t atari -T -Osir creature1.c
cc65 -t atari -T -Osir creature2.c
cc65 -t atari -T -Osir creature3.c
cc65 -t atari -T -Osir creature4.c
cc65 -t atari -T -Osir creature5.c
cc65 -t atari -T -Osir death1.c
cc65 -t atari -T -Osir dungeon1.c
cc65 -t atari -T -Osir dungeon2.c
cc65 -t atari -T -Osir dungeon3.c
cc65 -t atari -T -Osir dungeon4.c
cc65 -t atari -T -Osir dungeon5.c
cc65 -t atari -T -Osir dungeon6.c
cc65 -t atari -T -Osir dungeon7.c
cc65 -t atari -T -Osir dungeon8.c
cc65 -t atari -T -Osir eat1.c
cc65 -t atari -T -Osir generate1.c
cc65 -t atari -T -Osir generate2.c
cc65 -t atari -T -Osir generate3.c
cc65 -t atari -T -Osir guild.c
cc65 -t atari -T -Osir help.c
cc65 -t atari -T -Osir init.c
cc65 -t atari -T -Osir inventory1.c
cc65 -t atari -T -Osir inventory2.c
cc65 -t atari -T -Osir io.c
cc65 -t atari -T -Osir look.c
cc65 -t atari -T -Osir main.c
cc65 -t atari -T -Osir magictreasure1.c
cc65 -t atari -T -Osir magictreasure2.c
cc65 -t atari -T -Osir misc1.c
cc65 -t atari -T -Osir misc2.c
cc65 -t atari -T -Osir monsters.c
cc65 -t atari -T -Osir objdes.c
cc65 -t atari -T -Osir player.c
cc65 -t atari -T -Osir prayer.c
cc65 -t atari -T -Osir potions.c
cc65 -t atari -T -Osir printchar1.c
cc65 -t atari -T -Osir printchar2.c
cc65 -t atari -T -Osir quest1.c
cc65 -t atari -T -Osir quest2.c
cc65 -t atari -T -Osir recall.c
cc65 -t atari -T -Osir rnd.c
cc65 -t atari -T -Osir save.c
cc65 -t atari -T -Osir scrolls.c
cc65 -t atari -T -Osir setup.c
cc65 -t atari -T -Osir spells1.c
cc65 -t atari -T -Osir spells2.c
cc65 -t atari -T -Osir spells3.c
cc65 -t atari -T -Osir spells4.c
cc65 -t atari -T -Osir staffs.c
cc65 -t atari -T -Osir store1.c
cc65 -t atari -T -Osir store2.c
cc65 -t atari -T -Osir store3.c
cc65 -t atari -T -Osir storecmt.c
cc65 -t atari -T -Osir tables.c
cc65 -t atari -T -Osir tables2.c
cc65 -t atari -T -Osir tavern1.c
cc65 -t atari -T -Osir tavern2.c
cc65 -t atari -T -Osir treasure.c
cc65 -t atari -T -Osir wands.c
cc65 -t atari -T -Osir wizard1.c
cc65 -t atari -T -Osir wizard2.c
cc65 -t atari -T -Osir variable.c
cc65 -t atari -T -Osir vbxebank.c
cc65 -t atari -T -Osir vbxetext.c

ca65 -t atari bankcode0.s
ca65 -t atari bankcode1.s
ca65 -t atari bankcode2.s
ca65 -t atari cast.s
ca65 -t atari caveplace.s
ca65 -t atari create1.s
ca65 -t atari create2.s
ca65 -t atari creature1.s
ca65 -t atari creature2.s
ca65 -t atari creature3.s
ca65 -t atari creature4.s
ca65 -t atari creature5.s
ca65 -t atari death1.s
ca65 -t atari dungeon1.s
ca65 -t atari dungeon2.s
ca65 -t atari dungeon3.s
ca65 -t atari dungeon4.s
ca65 -t atari dungeon5.s
ca65 -t atari dungeon6.s
ca65 -t atari dungeon7.s
ca65 -t atari dungeon8.s
ca65 -t atari eat1.s
ca65 -t atari flashcart.s
ca65 -t atari generate1.s
ca65 -t atari generate2.s
ca65 -t atari generate3.s
ca65 -t atari guild.s
ca65 -t atari help.s
ca65 -t atari init.s
ca65 -t atari inventory1.s
ca65 -t atari inventory2.s
ca65 -t atari io.s
ca65 -t atari look.s
ca65 -t atari main.s
ca65 -t atari magictreasure1.s
ca65 -t atari magictreasure2.s
ca65 -t atari misc1.s
ca65 -t atari misc2.s
ca65 -t atari monsters.s
ca65 -t atari objdes.s
ca65 -t atari player.s
ca65 -t atari prayer.s
ca65 -t atari potions.s
ca65 -t atari printchar1.s
ca65 -t atari printchar2.s
ca65 -t atari returnbank.s
ca65 -t atari quest1.s
ca65 -t atari quest2.s
ca65 -t atari recall.s
ca65 -t atari rnd.s
ca65 -t atari save.s
ca65 -t atari scrolls.s
ca65 -t atari setup.s
ca65 -t atari spells1.s
ca65 -t atari spells2.s
ca65 -t atari spells3.s
ca65 -t atari spells4.s
ca65 -t atari staffs.s
ca65 -t atari store1.s
ca65 -t atari store2.s
ca65 -t atari store3.s
ca65 -t atari storecmt.s
ca65 -t atari tables.s
ca65 -t atari tables2.s
ca65 -t atari tavern1.s
ca65 -t atari tavern2.s
ca65 -t atari treasure.s
ca65 -t atari wands.s
ca65 -t atari wizard1.s
ca65 -t atari wizard2.s
ca65 -t atari variable.s
ca65 -t atari vbxebank.s
ca65 -t atari vbxetext.s

ld65 crt0.o cgetc.o bankcode0.o bankcode1.o bankcode2.o cast.o caveplace.o copyseg.o create1.o create2.o creature1.o creature2.o creature3.o creature4.o creature5.o death1.o dungeon1.o dungeon2.o dungeon3.o dungeon4.o dungeon5.o dungeon6.o dungeon7.o dungeon8.o eat1.o irqhand.o flashcart.o generate1.o generate2.o generate3.o guild.o help.o init.o inventory1.o inventory2.o io.o look.o magictreasure1.o magictreasure2.o main.o misc1.o misc2.o monsters.o objdes.o player.o prayer.o potions.o printchar1.o printchar2.o quest1.o quest2.o returnbank.o recall.o rnd.o save.o scrolls.o setup.o spells1.o spells2.o spells3.o spells4.o staffs.o store1.o store2.o store3.o storecmt.o tables.o tables2.o tavern1.o tavern2.o treasure.o wands.o wizard1.o wizard2.o variable.o vbxe.o vbxebank.o vbxetext.o zeropage.o --lib atari.lib -o moria.bin -C atari.cfg -m moria.map --dbgfile moria.dbg -Ln moria.lbl

