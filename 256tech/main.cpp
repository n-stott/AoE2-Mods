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

void twofiftysixify() {
  int N = dataset->Techs.size();
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
        || i != 47 || i != 664 ) {
      // if (effect.Name != "") std::cout << i << " " << effect.Name << std::endl;
      continue;
    }
    for(int j = 0; j < 255; ++j) {
      dataset->Techs.push_back(dataset->Techs[i]);
    }
  }
}

int main(int argc, char **argv)
{
  dataset = new genie::DatFile();
  dataset->setGameVersion(genie::GV_Cysion);
  dataset->load("../dat/empires2_x2_p1.dat");

  modify_farmrate();
//  add_thumbring(dataset);
  modify_arrowspeed();
  buff_chemistry();
  buff_petard();
  twofiftysixify();

  dataset->saveAs("./empires2_x2_p1.dat");
  return 0;
}
