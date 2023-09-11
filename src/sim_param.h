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

#ifndef SIM_PARAM_H_
#define SIM_PARAM_H_

#include <unordered_map>
#include "biodynamo.h"

namespace bdm {

// Parameters specific for this simulation
struct SimParam : public ParamGroup {
  BDM_PARAM_GROUP_HEADER(SimParam, 1);

  // Number of simulation steps
  int sims_steps = 1000;

  // Controls duration of symmetric division
  real_t intracellular_substance_ = 400;

  // differentiation probability
  real_t diff_prob = 0.5;

  real_t MZ_height = 150;  
  real_t L6_height = 130; 
  real_t L5_height = 110;  
  real_t L4_height = 90;   
  real_t L3_height = 70;   
  real_t L2_height = 50;   

  std::unordered_map<int, int> height_map = {{1, MZ_height}, {2, L6_height},
                                             {3, L5_height}, {4, L4_height},
                                             {5, L3_height}, {6, L2_height}};

};

}  // namespace bdm

#endif  // SIM_PARAM_H_
