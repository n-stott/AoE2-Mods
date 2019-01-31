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
#include <math.h>
#include <cassert>


genie::DatFile *dataset;
int mult = 3;

void multiply(genie::EffectCommand* command) {
  // std::cout <<"  "<< command->D << std::endl;
  if (command->C == 8 || command->C == 9) {
    double d = command->D;
    if (d < 0) {
      d = d+65536;
      command->D = ((int)d / 256)*256 + (mult*((int)d % 256) % 256);
      command->D -= 65536;
    }
    else {
      command->D = ((int)d / 256)*256 + (mult*((int)d % 256) % 256);
    }
  }
  else {
    command->D *= mult;
  }
}

void power(genie::EffectCommand* command) {
  if (command->C == 8 || command->C == 9) {
    command->D = ((int)command->D / 256)*256 + ( (int) pow((int)command->D % 256, mult) % 256 );
  }
  else {
    command->D = pow(command->D,mult);
  }
}

void double1(genie::EffectCommand* command) {
  if (command->A == 6) {
    // std::cout << command->A << " " << command->B << std::endl;
    return;
  }
  // if (command->A == 1) {
    std::cout << command->A << " " << command->B  
    << " " << command->C << " " << command->D << std::endl;
  // }
  multiply(command);
    std::cout << command->A << " " << command->B  
    << " " << command->C << " " << command->D << std::endl;
}
void double4(genie::EffectCommand* command) {
  multiply(command);
}
void double5(genie::EffectCommand* command) {
  power(command);
}
void double6(genie::EffectCommand* command) {
  power(command);
}
void double11(genie::EffectCommand* command) {
  multiply(command);
}
void double14(genie::EffectCommand* command) {
  multiply(command);
}
void double15(genie::EffectCommand* command) {
  power(command);
}
void double16(genie::EffectCommand* command) {
  power(command);
}
void double101(genie::EffectCommand* command) {
  multiply(command);
}
void double103(genie::EffectCommand* command) {
  multiply(command);
}

void doubleEffectify() {
  int N = dataset->Effects.size();
  for(int i = 0; i < N; ++i) {
    int P = dataset->Effects[i].EffectCommands.size();
    bool print = true;
    for(int j = 0; j < P; ++j) {

      genie::EffectCommand* command = &dataset->Effects[i].EffectCommands[j];

      if (command->Type == 1 || command->Type ==6 ) {
        if (command->A == 4) {
          std::cout << i << "  " << j << std::endl;
        }
      }

      // if(i == 695) {
      //   std::cout << P 
      //   << "  " <<  command->Type 
      //   << "  "<< command->A 
      //   << "  " << command->B << std::endl;
      // }

      
      if ( (command->Type == 1 || command->Type == 11 || command->Type == 101) 
        && command->B == 0 
        && command->D != 0 
        && command-> A != 6 
        && command->A != 66 
        && dataset->Effects[i].Name != "Non-Research" ) {
        // if (print) {std::cout << i << "  "  << dataset->Effects[i].Name << std::endl; print = false;}
      }

      if ( (command->Type == 1 || command->Type == 4 
      || command->Type == 11 || command->Type == 14
      || command->Type == 101 || command->Type == 103) 
        && command->D  < 0 
        && dataset->Effects[i].Name != "Non-Research" ) {
        // if (print) {std::cout << i << "  "  << dataset->Effects[i].Name << std::endl; print = false;}
      }

      switch(command->Type) {
        case 1:
          std::cout << i << std::endl;
          double1(command);
          break;
        case 4:
          double4(command);
          break;
        case 5:
          double5(command);
          break;
        case 6:
          double6(command);
          break;
        case 11:
          double11(command);
          break;
        case 14:
          double14(command);
          break;
        case 15:
          double15(command);
          break;
        case 16:
          double16(command);
          break;
        case 101:
          if (i==256) std::cout << dataset->Effects[i].Name << "  " << command->D <<  std::endl;
          double101(command);
          if (i==256) std::cout << dataset->Effects[i].Name << "  " << command->D <<  std::endl;
          break;
        case 103:
          double103(command);
          break;

      }
    }
  }
}


void doubleUpgradeIter(genie::Unit low_ref, genie::Unit up_ref, genie::Unit* low, genie::Unit* up, bool active) {
  if(!active || (low->Type != 70 && low->Type != 80) )return;
  // std::cout << low_ref.Name <<"  " << low_ref.HitPoints<< "  " << up_ref.Name <<"  " << up_ref.HitPoints << std::endl;
  // std::cout << low->Name << "  " << up->Name << std::endl;
  up->HitPoints = low->HitPoints + mult*(up_ref.HitPoints - low_ref.HitPoints);
  up->LineOfSight =low->LineOfSight +  mult*(up_ref.LineOfSight - low_ref.LineOfSight);
  up->GarrisonCapacity = low->GarrisonCapacity + mult*(up_ref.GarrisonCapacity - low_ref.GarrisonCapacity);
  up->Speed = low->Speed + mult*(up_ref.Speed - low_ref.Speed);
  for(int j = 0; j < up->Type50.Attacks.size(); ++j) {
    for(int i = 0; i < low->Type50.Attacks.size(); ++i) {
      if(low->Type50.Attacks[i].Class == up->Type50.Attacks[j].Class) {
        up->Type50.Attacks[j].Amount = low->Type50.Attacks[i].Amount + mult*(up_ref.Type50.Attacks[j].Amount - low_ref.Type50.Attacks[i].Amount);
       // br);
      }
    }
    // up->Type50.Attacks[j] *= 2;
  }
  for(int j = 0; j < up->Type50.Armours.size(); ++j) {
    for(int i = 0; i < low->Type50.Armours.size(); ++i) {
      if(low->Type50.Armours[i].Class == up->Type50.Armours[j].Class) {
        up->Type50.Armours[j].Amount = low->Type50.Armours[i].Amount + mult*(up_ref.Type50.Armours[j].Amount - low_ref.Type50.Armours[i].Amount);
        // break;
      }
    }
    // up->Type50.Attacks[j] *= 2;
  }
}


void doubleUpgrade() {
  std::vector<int> level;
  std::vector<std::pair<int, int>> upgrades;


  //gather upgrade table
  int ma = 0;
  int N = dataset->Effects.size();
  for(int i = 0; i < N; ++i) {
    int P = dataset->Effects[i].EffectCommands.size();
    for(int j = 0; j < P; ++j) {
      genie::EffectCommand* command = &dataset->Effects[i].EffectCommands[j];
      if ((command->Type == 3 || command->Type == 13) && command->A > 0 && command->B > 0 && dataset->Effects[i].Name != "Non-Research" ) {
        // std::cout << i << "  " << j << "  " << command->A << " " << command->B << "  " << dataset->Effects[i].Name << std::endl;
        // std::cout << low->Name << "  " << up->Name << std::endl;
        if ( std::find(upgrades.begin(), upgrades.end(), std::pair<int,int>(command->B, command->A)) != upgrades.end() ) {
          // std::cout << "DUPLICATE  " << i << "  " << dataset->Effects[i].Name << std::endl;
        }
        upgrades.push_back(std::pair<int,int>(command->A, command->B));
        ma = std::max((int)command->A, std::max(ma, (int)command->B));
      }
    }

  }
  // std::cout << "UPGRADES : " << upgrades.size() << std::endl;

  
  for(auto& u : upgrades) {
    for(auto& v : upgrades) {
      if( u.second == v.first && std::find(upgrades.begin(), upgrades.end(), std::pair<int,int>(u.first, v.second)) == upgrades.end() ) {
        // std::cout << u.first << "  " << u.second << "  " << v.first << "  " << v.second << std::endl;
      }
    }
  }

  level = std::vector<int>(ma, 0);
  for(std::pair<int,int>& u : upgrades) {
    level[u.second] = std::count_if(upgrades.begin(), upgrades.end(), [&u](std::pair<int, int> up){return up.second == u.second;});
  }
  // for(int i = 0; i < ma; ++i) {
  //   level.push_back(-1);
  // }
  // for(int l = 0; l < 20; ++l) {
  //   for(int i = 0; i < low.size(); ++i) {
  //     if(level[low[i]] == level[up[i]]) {
  //       level[up[i]]++;
  //     }
  //   }


  // }

  std::vector<genie::Unit> ref = dataset->Civs[0].Units;
  for(auto& u : upgrades) {
    // std::cout << level[u.first] << "  " << level[u.second] << "  " << ref[u.first].Name <<"  " << ref[u.first].HitPoints<< "  " << ref[u.second].Name <<"  " << ref[u.second].HitPoints << std::endl;
  }


// std::cout << "UPGRADING" << std::endl;


  for(auto& c : dataset->Civs) {
    for(int l = 0; l < 4; ++l) {
      for(auto& u : upgrades) {
        doubleUpgradeIter(ref[u.first], ref[u.second], &c.Units[u.first], &c.Units[u.second], level[u.first] == l && level[u.second] == l+1);
      }
    }
  }

  ref = dataset->Civs[0].Units;
  for(auto& u : upgrades) {
    // std::cout << level[u.first] << "  " << level[u.second] << "  " << ref[u.first].Name <<"  " << ref[u.first].HitPoints<< "  " << ref[u.second].Name <<"  " << ref[u.second].HitPoints << std::endl;
  }


}

void fixCouterExamples() {
  // Italiens Tech tree
  dataset->Effects[10].EffectCommands[8].D = 500*pow(0.85,mult);
  dataset->Effects[10].EffectCommands[9].D = 800*pow(0.85,mult);
  dataset->Effects[10].EffectCommands[10].D = 200*pow(0.85,mult);
  dataset->Effects[10].EffectCommands[11].D = 1000*pow(0.85,mult);
  dataset->Effects[10].EffectCommands[12].D = 800*pow(0.85,mult);
  dataset->Effects[10].EffectCommands[16].D = 230*pow(0.5,mult);
  dataset->Effects[10].EffectCommands[17].D = 100*pow(0.5,mult);

  //Guilds
  dataset->Effects[15].EffectCommands[0].D = 0.2*pow(0.75,mult);

  //Coinage
  dataset->Effects[23].EffectCommands[0].D = 0.3*pow(0.666,mult);

  //Byzantines
  dataset->Effects[256].EffectCommands[23].D = 800*pow(0.666, mult) -800;
  dataset->Effects[256].EffectCommands[24].D = 1000*pow(0.666, mult) -1000;

  //Turks
  dataset->Effects[263].EffectCommands[27].D = 250*pow(0.5,mult) -250;
  dataset->Effects[263].EffectCommands[28].D = 500*pow(0.5,mult)-500;
  dataset->Effects[263].EffectCommands[29].D = 450*pow(0.5,mult)-450;
  dataset->Effects[263].EffectCommands[30].D = 200*pow(0.5,mult)-200;
  dataset->Effects[263].EffectCommands[31].D = 400*pow(0.5,mult)-400;
  dataset->Effects[263].EffectCommands[32].D = 500*pow(0.5,mult)-500;
  dataset->Effects[263].EffectCommands[33].D = 400*pow(0.5,mult)-400;
  dataset->Effects[263].EffectCommands[34].D = 800*pow(0.5,mult)-800;
  dataset->Effects[263].EffectCommands[41].D = 525*pow(0.5,mult)-525;
  dataset->Effects[263].EffectCommands[42].D = 500*pow(0.5,mult)-500;

  // //Chinese
  dataset->Effects[302].EffectCommands[0].D = 3+3*mult;

  // //what ?
  dataset->Effects[349].EffectCommands[0].D = pow(0.9,mult);
  dataset->Effects[350].EffectCommands[0].D = pow(0.85,mult);

  // std::cout << dataset->Effects.size() << std::endl;

  dataset->Effects[351].EffectCommands[0].D = pow(0.8,mult);

  //Byzantines
  dataset->Effects[400].EffectCommands[0].D = pow(1.5,mult);

  //Spanish
  dataset->Effects[446].EffectCommands[24].D = pow(1.3,mult);  

  //Huns

  dataset->Effects[448].EffectCommands[31].D = 1000;

  //Mayans
  dataset->Effects[449].EffectCommands[35].D = pow(1.15,mult);
  dataset->Effects[449].EffectCommands[36].D = pow(1.15,mult);
  dataset->Effects[449].EffectCommands[37].D = pow(1.15,mult);
  dataset->Effects[449].EffectCommands[38].D = pow(1.15,mult);

  //Theocracy
  dataset->Effects[494].EffectCommands[0].D = 1;

  //Mayans
  dataset->Effects[586].EffectCommands[0].D = 3 + mult;

  dataset->Effects[651].EffectCommands[0].D = 42;

  dataset->Effects[689].EffectCommands[0].D = 1000*pow(0.5,mult)-1000;
  dataset->Effects[689].EffectCommands[1].D = 750*pow(0.5,mult)-750;
  dataset->Effects[689].EffectCommands[2].D = 200*pow(0.5,mult)-200;
  dataset->Effects[689].EffectCommands[3].D = 120*pow(0.5,mult)-120;
  dataset->Effects[689].EffectCommands[4].D = 120*pow(0.5,mult)-120;
  dataset->Effects[689].EffectCommands[5].D = 140*pow(0.5,mult)-140;
  dataset->Effects[689].EffectCommands[6].D = 475*pow(0.5,mult)-475;
  dataset->Effects[689].EffectCommands[7].D = 325*pow(0.5,mult)-325;
  dataset->Effects[689].EffectCommands[8].D = 200*pow(0.5,mult)-200;
  dataset->Effects[689].EffectCommands[9].D = 1000*pow(0.5,mult)-1000;
  dataset->Effects[689].EffectCommands[10].D = 350*pow(0.5,mult)-350;

  dataset->Effects[694].EffectCommands[0].D = 1000;

}

float amount(int tech, int type) {
  for(auto& rc : dataset->Techs[tech].ResourceCosts) {
    if (rc.Type == type) {
      return rc.Amount;
    }
  }
  std::cerr << "Cannot find tech " << tech << " cost resource " << type;
  return 0;
}

void correctNegatives() {
  int i = 0;
  for(genie::Effect& eff : dataset->Effects) {
    bool print = true;
    int j = 0;
    for(genie::EffectCommand& com : eff.EffectCommands) {
        if ( (com.Type == 1 || com.Type == 4 
          || com.Type == 11 || com.Type == 14
          || com.Type == 101 || com.Type == 103 )
          && com.D  < 0 && eff.Name != "Non-Research" ) {
            float cost  = 0;
            
            if (i == 10 && com.Type == 4)  {
              cost = dataset->Civs[0].Units[13].Creatable.ResourceCosts[0].Amount;
              float b = -com.D/mult;
              com.D =  cost*pow(1-b/cost, mult) - cost;
            }            

            if (i == 10 && com.Type == 101)  {
              cost = amount(com.A, com.B);
              float b = -com.D/mult;
              com.D =  cost*pow(1-b/cost, mult) - cost;
            }
            

            if (i == 354 && com.Type == 4)  {
              cost = dataset->Civs[0].Units[84].Creatable.ResourceCosts[0].Amount;
              float b = -com.D/mult;
              com.D =  cost*pow(1-b/cost, mult) - cost;
            }  

            if (i == 665) {
              com.D = -20;
            }

            if (i == 674) {
              cost = dataset->Techs[com.A].ResearchTime;
              float b = -com.D/mult;
              com.D =  cost*pow(1-b/cost, mult) - cost;
            }

            // std::cout 
            //   << i << "  " 
            //   << j << "  "  
            //   << eff.Name << "  " 
            //   << com.D  << "  " 
            //   << cost << "  "
            //   << cost + com.D
            // << std::endl;
      }
      ++j;
    } 
    ++i;
  }
      
}

void changeKarambit() {
  genie::Unit* karambit = &dataset->Civs[0].Units[1123];
  for(int i = 0; i < 3; ++i) {
    karambit->ResourceStorages[i].Amount = pow(0.5, mult-1)*karambit->ResourceStorages[i].Amount;
  }
  genie::Unit* ekarambit = &dataset->Civs[0].Units[1125];
  for(int i = 0; i < 3; ++i) {
    ekarambit->ResourceStorages[i].Amount = pow(0.5, mult-1)*ekarambit->ResourceStorages[i].Amount;
  }
}

int main(int argc, char **argv)
{
  dataset = new genie::DatFile();
  dataset->setGameVersion(genie::GV_Cysion);
  dataset->load("../dat/empires2_x2_p1.dat");

  doubleEffectify();
  doubleUpgrade();
  fixCouterExamples();
  correctNegatives();
  changeKarambit();

  dataset->Effects[676].EffectCommands[0].Type = -1;
  dataset->Effects[676].EffectCommands[0].A = -1;
  dataset->Effects[676].EffectCommands[0].B = -1;


  // dataset->Effects.erase( dataset->Effects.begin()+676);

  dataset->saveAs("./empires2_x2_p1.dat");
  return 0;
}
