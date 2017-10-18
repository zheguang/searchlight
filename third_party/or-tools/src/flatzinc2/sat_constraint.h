// Copyright 2010-2013 Google
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef OR_TOOLS_FLATZINC_SAT_CONSTRAINT_H_
#define OR_TOOLS_FLATZINC_SAT_CONSTRAINT_H_

#include "constraint_solver/constraint_solver.h"

namespace operations_research {
class SatPropagator;

SatPropagator* MakeSatPropagator(Solver* solver);

bool AddBoolEq(SatPropagator* sat, IntExpr* left, IntExpr* right);

bool AddBoolLe(SatPropagator* sat, IntExpr* left, IntExpr* right);

bool AddBoolNot(SatPropagator* sat, IntExpr* left, IntExpr* right);

bool AddBoolAndArrayEqVar(SatPropagator* sat, const std::vector<IntVar*>& vars,
                          IntExpr* target);

bool AddBoolOrArrayEqVar(SatPropagator* sat, const std::vector<IntVar*>& vars,
                         IntExpr* target);

bool AddSumBoolArrayGreaterEqVar(SatPropagator* sat,
                                 const std::vector<IntVar*>& vars, IntExpr* target);

bool AddSumBoolArrayLessEqKVar(SatPropagator* sat, const std::vector<IntVar*>& vars,
                               IntExpr* target);

bool AddBoolAndEqVar(SatPropagator* sat, IntExpr* left, IntExpr* right,
                     IntExpr* target);

bool AddBoolIsNEqVar(SatPropagator* sat, IntExpr* left, IntExpr* right,
                     IntExpr* target);

bool AddBoolIsLeVar(SatPropagator* sat, IntExpr* left, IntExpr* right,
                    IntExpr* target);

bool AddBoolOrEqVar(SatPropagator* sat, IntExpr* left, IntExpr* right,
                    IntExpr* target);

bool AddBoolIsEqVar(SatPropagator* sat, IntExpr* left, IntExpr* right,
                    IntExpr* target);

bool AddBoolOrArrayEqualTrue(SatPropagator* sat, const std::vector<IntVar*>& vars);

bool AddBoolAndArrayEqualFalse(SatPropagator* sat, const std::vector<IntVar*>& vars);

bool AddAtMostOne(SatPropagator* sat, const std::vector<IntVar*>& vars);

bool AddAtMostNMinusOne(SatPropagator* sat, const std::vector<IntVar*>& vars);

bool AddArrayXor(SatPropagator* sat, const std::vector<IntVar*>& vars);

void DeclareVariable(SatPropagator* sat, IntVar* var);
}  // namespace operations_research
#endif  // OR_TOOLS_FLATZINC_SAT_CONSTRAINT_H_
