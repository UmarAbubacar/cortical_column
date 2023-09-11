// -----------------------------------------------------------------------------
//
// Copyright (C) 2021 CERN & University of Surrey for the benefit of the
// BioDynaMo collaboration. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------

#ifndef GENERN_H_
#define GENERN_H_

#include "biodynamo.h"
#include "core/behavior/behavior.h"

#include "Neurons.h"
#include "sim_param.h"

namespace bdm {

struct SingleDivision : public Behavior {
  BDM_BEHAVIOR_HEADER(SingleDivision, Behavior, 1);
  SingleDivision() {}
  virtual ~SingleDivision() {}

  void Run(Agent* agent) override {
    auto* cell = dynamic_cast<MyNeuron*>(agent);
    if (cell->GetDiameter() <= cell->GetThreshold()) {
      cell->ChangeVolume(cell->GetGrowthRate());
    } else {
      // cell->Divide();
      auto* daughter = dynamic_cast<MyNeuron*>(cell->Divide());
      daughter->SetState(cell->GetState());

      // Remove this behavior from the cell
      cell->RemoveBehavior(cell->GetAllBehaviors()[1]);

      // move on to next stage
      cell->SetState(cell->GetState() + 1);
    }
  }
};

struct state : public Behavior {
  BDM_BEHAVIOR_HEADER(state, Behavior, 1);
  state() {}
  explicit state(int cell_type) { state_cell_type_ = cell_type; }
  virtual ~state() {}

  void Run(Agent* agent) override {
    auto* random = Simulation::GetActive()->GetRandom();
    auto* sparam = Simulation::GetActive()->GetParam()->Get<SimParam>();

    auto* cell = dynamic_cast<MyNeuron*>(agent);

    if (cell->GetAllBehaviors().size() < 3) {
      // check if cell differentiates
      if (random->Uniform(0, 1) < sparam->diff_prob) {
        // remove this behavior from the cell
        cell->RemoveBehavior(cell->GetAllBehaviors()[1]);

        // set migration substance to MZ height using state_cell_type_ and
        // height_map
        std::unordered_map<int, int>::const_iterator got =
            sparam->height_map.find(state_cell_type_);
        if (got == sparam->height_map.end()) {
          std::cout << "Error: cell type not found in height map" << std::endl;
          exit(1);
        }
        cell->AddBehavior(new DifferentiatedNeuron());
        cell->SetCellType(state_cell_type_);
        
        cell->SetMigrationSubstance(got->second);
        cell->AddBehavior(new Chemotaxis("substance_kalium", 3));

      } else {
        // remove this behavior from the cell
        cell->RemoveBehavior(cell->GetAllBehaviors()[1]);

        // added another behavior total of 2 (state, single growth)
        if (cell->GetAllBehaviors().size() < 3) {
          cell->AddBehavior(new SingleDivision());
        }
      }
    }
  }

 private:
  int state_cell_type_;
};

struct SymmetricDivision : public Behavior {
  BDM_BEHAVIOR_HEADER(SymmetricDivision, Behavior, 1);
  SymmetricDivision() { AlwaysCopyToNew(); }
  virtual ~SymmetricDivision() {}

  void Run(Agent* agent) override {
    auto* cell = dynamic_cast<MyNeuron*>(agent);

    if (cell->GetDiameter() <= cell->GetThreshold() && !is_finished_) {
      cell->ChangeVolume(cell->GetGrowthRate());
    } else {
      auto* daughter = cell->Divide();
      auto* d_neuron = static_cast<MyNeuron*>(daughter);
      d_neuron->SetIntracellularSubstance(cell->GetIntracellularSubstance());
    }

    // Decay of intracellular substance
    cell->SetIntracellularSubstance(cell->GetIntracellularSubstance() - 1);

    if (cell->GetIntracellularSubstance() < cell->GetInternalThreshold()) {
      is_finished_ = true;
      cell->RemoveBehavior(cell->GetAllBehaviors()[1]);
      // cell->AddBehavior(new s1());
      cell->SetState(cell->GetState() + 1);
    }
  }

 private:
  bool is_finished_ = false;
};

struct GRN : public Behavior {
  BDM_BEHAVIOR_HEADER(GRN, Behavior, 1);
  GRN() { AlwaysCopyToNew(); }
  virtual ~GRN() {}

  void Run(Agent* agent) override {
    auto* cell = dynamic_cast<MyNeuron*>(agent);

    // switch statement that adds the correct behavior to the neuron based on
    if (cell->GetAllBehaviors().size() < 2) {
      switch ((int)cell->GetState()) {
        case 0:
          cell->AddBehavior(new SymmetricDivision());
          break;
        case 1:
          cell->AddBehavior(new state(1));
          break;
        case 2:
          cell->AddBehavior(new state(2));
          break;
        case 3:
          cell->AddBehavior(new state(3));
          break;
        case 4:
          cell->AddBehavior(new state(4));
          break;
        case 5:
          cell->AddBehavior(new state(5));
          break;
        case 6:
          cell->AddBehavior(new state(6));
          break;
      }
    }
  }
};

}  // namespace bdm

#endif  // GENERN_H_
