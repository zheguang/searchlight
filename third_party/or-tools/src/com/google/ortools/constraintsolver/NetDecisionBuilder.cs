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

using System;

namespace Google.OrTools.ConstraintSolver
{
/**
 * This class acts as a intermediate step between a c++ decision
 * builder and a .Net one. Its main purpose is to catch the .Net
 * exception launched when a failure occurs during the Next() call,
 * and to return silently a System.ApplicationException that will
 * propagate the failure back to the C++ code.
 *
 */
public class NetDecisionBuilder : DecisionBuilder
{
  /**
   * This methods wraps the calls to next() and catches fail exceptions.
   */
  public override Decision NextWrap(Solver solver)
  {
    try
    {
      return Next(solver);
    }
    catch (ApplicationException e)
    {
      return solver.MakeFailDecision();
    }
  }
  /**
   * This is the new method to subclass when defining a .Net decision builder.
   */
  public virtual Decision Next(Solver solver)
  {
    return null;
  }
}
}  // namespace Google.OrTools.ConstraintSolver

