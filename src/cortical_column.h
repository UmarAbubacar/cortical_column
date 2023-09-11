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
#ifndef cortical_column_H_
#define cortical_column_H_

#include "biodynamo.h"
#include "neuroscience/neuroscience.h"

#include "GeneRN.h"
#include "Neurons.h"
#include "sim_param.h"

#include <fstream>
#include <iostream>

namespace bdm {

enum Substances { kKalium };

/// Define extracellular substance for migration
inline void CreateExtracellularSubstances(const Param* p) {
  using MI = ModelInitializer;

  // create migration grid
  MI::DefineSubstance(kKalium, "substance_kalium", 50, 0.1, 25);
  auto c_initializer = GaussianBand(p->max_bound, 300, Axis::kYAxis);
  MI::InitializeSubstance(kKalium, c_initializer);
}

/// Prints the number of neurons of each cell type to the terminal
inline void PrintNeuronNumbers(Simulation& sim) {
  auto* rm = sim.GetResourceManager();
  // get numbers of neurons with different cell_type_'s
  int num_MZ = 0;
  int num_L6 = 0;
  int num_L5 = 0;
  int num_L4 = 0;
  int num_L3 = 0;
  int num_L2 = 0;

  rm->ForEachAgent([&](Agent* agent) {
    auto* cell = dynamic_cast<MyNeuron*>(agent);
    if (cell != nullptr) {
      auto cell_type = cell->GetCellType();
      if (cell_type == 1) {
        num_MZ++;
      } else if (cell_type == 2) {
        num_L6++;
      } else if (cell_type == 3) {
        num_L5++;
      } else if (cell_type == 4) {
        num_L4++;
      } else if (cell_type == 5) {
        num_L3++;
      } else if (cell_type == 6) {
        num_L2++;
      }
    }
  });

  std::cout << "TOTAL CELL NUMBERS: " << std::endl;
  std::cout << "Number of MZ neurons: " << num_MZ << std::endl;
  std::cout << "Number of L2 neurons: " << num_L2 << std::endl;
  std::cout << "Number of L3 neurons: " << num_L3 << std::endl;
  std::cout << "Number of L4 neurons: " << num_L4 << std::endl;
  std::cout << "Number of L5 neurons: " << num_L5 << std::endl;
  std::cout << "Number of L6 neurons: " << num_L6 << std::endl;
}

/// Adds a progenitor cell to the simulation
inline void AddProgenitor(const Real3& position, int diameter, int mass) {
  auto* sim = Simulation::GetActive();
  auto* param = sim->GetParam();
  auto* sparam = param->Get<SimParam>();

  MyNeuron* soma = new MyNeuron(position);
  soma->SetDiameter(diameter);
  soma->SetMass(mass);
  soma->SetIntracellularSubstance(sparam->intracellular_substance_);

  Simulation::GetActive()->GetResourceManager()->AddAgent(soma);

  soma->AddBehavior(new GRN());
}

inline int Simulate(int argc, const char** argv) {
  // Adding space edge of but to be used in larger use case.
  auto set_param = [](Param* param) {
    param->use_progress_bar = true;
    param->min_bound = 0;
    param->max_bound = 1000;
    param->export_visualization = true;
    param->visualization_interval = 1;
  };
  Param::RegisterParamGroup(new SimParam());

  neuroscience::InitModule();

  Simulation simulation(argc, argv, set_param);
  auto* param = simulation.GetParam();
  auto* sparam = param->Get<SimParam>();

  // Set up the extracellular substances
  CreateExtracellularSubstances(simulation.GetParam());

  // Create grid of neurons nxn from center of simulation space
  int n = 4;
  double grid_spacing = 50;
  double x_offset = (param->max_bound / 2) - (n / 2) * grid_spacing;
  double y_offset = (param->max_bound / 2) - (n / 2) * grid_spacing;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double x = x_offset + i * grid_spacing;
      double y = y_offset + j * grid_spacing;
      AddProgenitor({x, 0, y}, 20, 10);
    }
  }

  simulation.GetScheduler()->Simulate(sparam->sims_steps);
  PrintNeuronNumbers(simulation);
  std::cout << "Simulation completed successfully!" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // cortical_column_H_
