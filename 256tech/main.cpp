#include <iostream>
#include <vector>
#include <genie/resource/PalFile.h>
#include <fstream>
#include <genie/resource/SlpFile.h>
#include <genie/resource/SlpFrame.h>
#include "genie/resource/DrsFile.h"
#include "genie/resource/SlpFile.h"
#include "genie/resource/Color.h"
#include <genie/script/ScnFile.h>
#include <genie/file/Compressor.h>
#include <genie/dat/DatFile.h>
#include <boost/iostreams/copy.hpp>
#include "genie/Types.h"

#include <genie/dat/TechageEffect.h>
#include <genie/dat/Civ.h>
#include <bitset>
#include <genie/dat/Techage.h>
#include <string>


genie::DatFile *dataset;


/*
 Change farm gather rate
*/
void modify_farmrate() {
  genie::EffectCommand farm_gather_rate;
  std::vector<int> As {214, 259, 226, 50, 215};


  farm_gather_rate.Type = 5;
  farm_gather_rate.B = -1;
  farm_gather_rate.C = 13;
  farm_gather_rate.D = 1.15;

  for(auto& a : As) {
    farm_gather_rate.A = a;
    dataset->Effects[14].EffectCommands.push_back(farm_gather_rate);
  }
}

/*
  Add thumb ring 256 times
*/
void add_thumbring() {
  for(int i = 0; i < 256; ++i) {
    dataset->Techs.push_back(dataset->Techs[437]);
  }
}

/*
  Modify arrow speed with thumbring
*/

void modify_arrowspeed() {
  genie::Civ c = dataset->Civs[0];
  std::vector<int> arrow_ids{};
  for(int i = 0; i < c.Units.size(); ++i) {
    if (c.Units[i].LanguageDLLName == 5067)
      // std::cout << i << "  " << c.Units[i].Name << std::endl;
      arrow_ids.push_back(i);
  }
  genie::EffectCommand arrowspeed;

  arrowspeed.Type = 4;
  arrowspeed.B = -1;
  arrowspeed.C = 5;
  arrowspeed.D = 0.25;

  for(auto& a : arrow_ids) {
    arrowspeed.A = a;
    dataset->Effects[451].EffectCommands.push_back(arrowspeed);
  }  
}

void buff_chemistry() {
  int N = dataset->Effects[47].EffectCommands.size();
  for(int i = 0; i < N; ++i) {
    if (dataset->Effects[47].EffectCommands[i].Type == 4) {
      dataset->Effects[47].EffectCommands[i].D += 1;
      // int att = dataset->Effects[47].EffectCommands[i].D;
      // std::cout << (att >> 8)  << "   " << (att % 256) << std::endl;
    }
  }
  genie::EffectCommand petard_attack;

  petard_attack.Type = 4;
  petard_attack.A = -1;
  petard_attack.B = 35;
  petard_attack.C = 9;
  petard_attack.D = 4*256 + 2;

  dataset->Effects[47].EffectCommands.push_back(petard_attack);
}

void buff_petard() {
  genie::EffectCommand blastradius;

  blastradius.Type = 4;
  blastradius.A = -1;
  blastradius.B = 35;
  blastradius.C = 22;
  blastradius.D = 0.5;

  dataset->Effects[604].EffectCommands.push_back(blastradius);
}

std::vector<int> quantity() {
  int N = dataset->Techs.size();
  std::vector<int> q;
  for(int i = 0; i < N; ++i) {
    q.push_back(255);
  }
  q[5] = 16;
  q[6] = 15; // drill
  q[7] = 15; // mahouts
  q[39] = 20; // husbandry
  q[48] = 40; // caravan
  q[50] = 3; //masonry 17 is the limit in a vacuum
  q[51] = 3;
  q[213] = 20;
  q[215] = 30;
  q[249] = 1;
  q[252] = 20;
  //q[377] = ??; //Siege engineers - looks problematic, but never encountered...
  q[379] = 4; //hoardings - 9 is the tipping point is a vacuum
  q[380] = 5;
  q[437] = 20;
  q[462] = 9;
  q[486] = 30;
  return q;
}

void find_mults() {
  for(int i = 0; i < dataset->Techs.size(); ++i) {
    genie::Effect* eff = &dataset->Effects[dataset->Techs[i].EffectID];
    bool prob = false;
    for(int j = 0; j < eff->EffectCommands.size(); ++j) {
      int t = eff->EffectCommands[j].Type;
      prob |= (t == 5 || t == 6 || t == 15 || t == 16) && (eff->EffectCommands[j].D > 1);
    }
    if (prob 
      && (eff->Name != "Non-Research") 
      && (eff->Name.find("Bonus") == std::string::npos ))
     std::cout << i << "  " << dataset->Techs[i].Name << "\n   "
     << dataset->Techs[i].EffectID << "  " << eff->Name << std::endl;
  }
}

void twofiftysixify() {
  int N = dataset->Techs.size();
  std::vector<int> q = quantity();
  for(int i = 0; i < N; ++i) {
 //   std::cout << i << "  " << N << std::endl;
    genie::Effect effect = dataset->Effects[dataset->Techs[i].EffectID];
    if ( dataset->Techs[i].EffectID == -1
        || effect.Name.find("C-Bonus") != std::string::npos 
        || effect.EffectCommands.size() == 0
        || effect.EffectCommands[0].Type == 2
        || effect.EffectCommands[0].Type == 3
        || effect.EffectCommands[0].Type == 12
        || effect.EffectCommands[0].Type == 13
        || effect.EffectCommands[0].Type == 102
        || i == 20 || i == 47 || i == 664 || (101 <= i && i <= 104)
        || i == 71 || i == 128 || i == 658 || (669 <= i && i <= 671)
        || i == 234 || i == 304 || i == 385 || i == 386
        || i == 387 || i == 393 || i == 408 || (452 <= i && i <= 455)
        || i == 600 || i == 612 || i == 610 || i == 611 || i == 353
        || i == 28
        ) {
      // if (effect.Name != "") std::cout << i << " " << effect.Name << std::endl;
      continue;
    }
    std::cout << i << "  " << dataset->Techs[i].Name << std::endl;
    for(int j = 0; j < q[i]; ++j) {
      dataset->Techs.push_back(dataset->Techs[i]);
    }
  }
}

int main(int argc, char **argv)
{
  dataset = new genie::DatFile();
  dataset->setGameVersion(genie::GV_Cysion);
  dataset->load("../dat/empires2_x2_p1.dat");

  // find_mults();

  modify_farmrate();
  modify_arrowspeed();
  buff_chemistry();
  buff_petard();
  twofiftysixify();

  dataset->saveAs("./empires2_x2_p1.dat");
  return 0;
}
