/// \file Visits.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/TupleTree/VisitsImpl.h"

template const model::Type *
getByPath<const model::Type, const model::Binary>(const TupleTreePath &Path,
                                                  const model::Binary &M);

template model::Type *
getByPath<model::Type, const model::Binary>(const TupleTreePath &Path,
                                            const model::Binary &M);

template model::Type *
getByPath<model::Type, model::Binary>(const TupleTreePath &Path,
                                      model::Binary &M);
