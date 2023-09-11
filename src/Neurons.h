#ifndef NEURONS_H_
#define NEURONS_H_

#include <vector>
#include "biodynamo.h"
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

#include "core/agent/cell_division_event.h"
#include "core/behavior/behavior.h"
#include "neuroscience/neuroscience.h"

namespace bdm {

enum States { s_0, s_1, s_2, s_3, s_4, s_5, s_6 };

class MyNeuron : public neuroscience::NeuronSoma {
  BDM_AGENT_HEADER(MyNeuron, neuroscience::NeuronSoma, 1);

 public:
  MyNeuron() {}
  explicit MyNeuron(const Real3& position) : Base(position) {}
  virtual ~MyNeuron() {}

  // getters and setters for data members
  real_t GetThreshold() const { return threshold_; }
  real_t GetGrowthRate() const { return growth_rate_; }

  real_t GetIntracellularSubstance() const { return intracellular_substance_; }
  void SetIntracellularSubstance(real_t intracellular_substance) {
    intracellular_substance_ = intracellular_substance;
  }

  real_t GetInternalThreshold() const { return internal_threshold_; }

  real_t GetMigrationThreshold() const { return migration_threshold_; }

  real_t GetMigrationSubstance() const { return migration_substance_; }
  void SetMigrationSubstance(real_t migration_substance) {
    migration_substance_ = migration_substance;
  }

  real_t GetState() const { return state_; }
  void SetState(real_t state) { state_ = state; }

  bool GetMigration() const { return migration_; }
  void SetMigration(bool migration) { migration_ = migration; }

  int GetCellType() const { return cell_type_; }
  void SetCellType(int cell_type) { cell_type_ = cell_type; }

 private:
  // for cell growth and division
  real_t threshold_ = 10;
  real_t growth_rate_ = 300;

  // for symmetric division
  real_t intracellular_substance_;
  real_t internal_threshold_ = 20;

  // for signalling migration
  bool migration_ = true;
  real_t migration_substance_;
  real_t migration_threshold_ = 5;

  // for identifying cell type
  real_t state_ = States::s_0;
  real_t cell_type_ = 0;
};

class DifferentiatedNeuron : public Behavior {
  BDM_BEHAVIOR_HEADER(DifferentiatedNeuron, Behavior, 1);

  void Run(Agent* agent) override {
    auto* cell = dynamic_cast<MyNeuron*>(agent);

    if (cell->GetMigration()) {
      // internal substance clock to limit migration
      cell->SetMigrationSubstance(cell->GetMigrationSubstance() - 1);

      if (cell->GetMigrationSubstance() <= cell->GetMigrationThreshold()) {
        cell->SetMigration(false);
        cell->RemoveBehavior(
            cell->GetAllBehaviors()[2]);  // removed migration behaviour
      }
    }
  }
};

}  // namespace bdm

#endif  // NEURONS_H_
