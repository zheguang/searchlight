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
#ifndef OR_TOOLS_LINEAR_SOLVER_MODEL_EXPORTER_H_
#define OR_TOOLS_LINEAR_SOLVER_MODEL_EXPORTER_H_

#include "base/hash.h"
#include <string>
#include "base/macros.h"
#include "base/hash.h"

namespace operations_research {

class MPConstraint;
class MPObjective;
class MPVariable;

namespace new_proto {
class MPModelProto;
}  // namespace new_proto

class MPModelProtoExporter {
 public:
  // The argument must live as long as this class is active.
  explicit MPModelProtoExporter(const new_proto::MPModelProto& proto);

  // Outputs the current model (variables, constraints, objective) as a std::string
  // encoded in the so-called "CPLEX LP file format" as generated by SCIP.
  // The LP file format is easily readable by a human.
  // Returns false if some error has occurred during execution.
  // If obfuscated is true, the variable and constraint names of proto_
  // are not used.  Variable and constraint names of the form "V12345"
  // and "C12345" are used instead.
  // For more information about the different LP file formats:
  // http://lpsolve.sourceforge.net/5.5/lp-format.htm
  // The following give a reasonable idea of the CPLEX LP file format:
  // http://lpsolve.sourceforge.net/5.5/CPLEX-format.htm
  // http://tinyurl.com/cplex-lp-format
  // http://www.gurobi.com/documentation/5.1/reference-manual/node871
  bool ExportModelAsLpFormat(bool obfuscated, std::string* model_str);

  // Outputs the current model (variables, constraints, objective) as a std::string
  // encoded in MPS file format, using the "fixed" MPS format if possible,
  // and the "free" MPS format otherwise.
  // Returns false if some error has occurred during execution.
  // If fixed_format is true, the method tries to use the MPS fixed format (the
  // use of which is discouraged as coefficients are printed with less
  // precision). If it is not possible to use the fixed format, the method falls
  // back to the so-called "free format".
  // If obfuscated is true, the variable and constraint names of proto_
  // are not used.  Variable and constraint names of the form "V12345"
  // and "C12345" are used instead.
  // For more information about the MPS format:
  // http://en.wikipedia.org/wiki/MPS_(format)
  // A close-to-original description coming from OSL:
  // http://tinyurl.com/mps-format-by-osl
  // A recent description from CPLEX:
  // http://tinyurl.com/mps-format-by-cplex
  // CPLEX extensions:
  // http://tinyurl.com/mps-extensions-by-cplex
  // Gurobi's description:
  // http://www.gurobi.com/documentation/5.1/reference-manual/node869
  bool ExportModelAsMpsFormat(bool fixed_format, bool obfuscated,
                              std::string* model_str);

  // Checks the validity of a variable or constraint name.
  // Used by MPSolver::CheckAllNamesValidity and
  // MPModelProtoExporter::CheckAllNamesValidity.
  // Returns false if:
  //  - name is empty.
  //  - name contains more than 255 characters.
  //  - the first character of name is one of "$.0123456789".
  //  - name contains space or a character in "+-*/<>=:\".
  // If name is empty, a message is issued to LOG(DFATAL). For all the other
  // cases which return false, a message is issued to LOG(WARNING).
  static bool CheckNameValidity(const std::string& name);

 private:
  // This scans the Model proto and sets up all the internal data structures
  // used for lookups.
  // Return false if the Model proto is inconsistent (duplicate names for
  // instance.)
  // It is called by ExportModelAsLpFormat and ExportModelAsMpsFormat.
  bool Setup();

  // Returns true if all constraint and variable names are valid according to
  // the criteria of MPSolver::CheckNameValidity().
  bool CheckAllNamesValidity() const;

  // If obfuscated is false and the variable number var_index has a name,
  // returns the variable name, otherwise returns a name in the form
  // "V00123" where "00123" corresponds to the variable number. The format
  // width of the number depends on the number of variables in the model.
  std::string GetVariableName(int var_index) const;

  // If use_obfuscated_names_ is false and the constraint number cst_index has
  // a name, returns the constraint name, otherwise returns a name in the form
  // "C00123" where "00123" corresponds to the constraint number. The format
  // width of the number depends on the number of constraints in the model.
  std::string GetConstraintName(int cst_index) const;

  // Returns true when the fixed MPS format can be used.
  // The fixed format is used when the variable and constraint names do not
  // exceed 8 characters. In the case of an obfuscated file, this means that
  // the maximum number of digits for constraints and variables is limited to 7.
  bool CanUseFixedMpsFormat() const;

  // Appends a general "Comment" section with useful metadata about the model
  // to "output".
  // Note(user): there may be less variables in output than in the original
  // model, as unused variables are not shown by default. Similarly, there
  // may be more constraints in a .lp file as in the original model as
  // a constraint lhs <= term <= rhs will be output as the two constraints
  // term >= lhs and term <= rhs.
  void AppendComments(const std::string& separator, std::string* output) const;

  // Clears "output" and writes a term to it, in "Lp" format. Returns false on
  // error (for example, var_index is out of range).
  bool WriteLpTerm(int var_index, double coefficient, std::string* output) const;

  // Appends a pair name, value to "output", formatted to comply with the MPS
  // standard.
  void AppendMpsPair(const std::string& name, double value, std::string* output) const;

  // Appends the head of a line, consisting of an id and a name to output.
  void AppendMpsLineHeader(const std::string& id, const std::string& name,
                           std::string* output) const;

  // Same as AppendMpsLineHeader. Appends an extra new-line at the end the
  // std::string pointed to by output.
  void AppendMpsLineHeaderWithNewLine(const std::string& id, const std::string& name,
                                      std::string* output) const;

  // Appends an MPS term in various contexts. The term consists of a head name,
  // a name, and a value. If the line is not empty, then only the pair
  // (name, value) is appended. The number of columns, limited to 2 by the MPS
  // format is also taken care of.
  void AppendMpsTermWithContext(const std::string& head_name, const std::string& name,
                                double value, std::string* output);

  // Appends a new-line if two columns are already present on the MPS line.
  // Used by and in complement to AppendMpsTermWithContext.
  void AppendNewLineIfTwoColumns(std::string* output);

  // When 'integrality' is true, appends columns corresponding to integer
  // variables. Appends the columns for non-integer variables otherwise.
  // The sparse matrix must be passed as a vector of columns ('transpose').
  void AppendMpsColumns(bool integrality,
                        const std::vector<std::vector<std::pair<int, double>>>& transpose,
                        std::string* output);

  // Appends a line describing the bound of a variablenew-line if two columns
  // are already present on the MPS line.
  // Used by and in complement to AppendMpsTermWithContext.
  void AppendMpsBound(const std::string& bound_type, const std::string& name,
                      double value, std::string* output) const;

  const new_proto::MPModelProto& proto_;

  // Maps a variable id to its index in the protobuf.
  hash_map<std::string, int> var_id_to_index_map_;

  // Number of integer variables in proto_.
  int num_integer_variables_;

  // Number of binary variables in proto_.
  int num_binary_variables_;

  // Number of continuous variables in proto_.
  int num_continuous_variables_;

  // Number of decimal digits needed to print the largest variable number.
  int num_digits_for_variables_;

  // Number of decimal digits needed to print the largest constraint number.
  int num_digits_for_constraints_;

  // Current MPS file column number.
  int current_mps_column_;

  // True is the fixed MPS format shall be used.
  bool use_fixed_mps_format_;

  // True if the variable and constraint names will be obfuscated.
  bool use_obfuscated_names_;

  // True if Setup was called on the current proto_.
  bool setup_done_;

  DISALLOW_COPY_AND_ASSIGN(MPModelProtoExporter);
};

}  // namespace operations_research

#endif  // OR_TOOLS_LINEAR_SOLVER_MODEL_EXPORTER_H_
