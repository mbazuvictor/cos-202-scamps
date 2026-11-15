#include "model/GeneralPatient.h"
#include "utils/Constants.h"

namespace scapms {

std::string GeneralPatient::category() const { return constants::CATEGORY_GENERAL; }
std::string GeneralPatient::describe() const { return "Normal priority - regular consultation"; }
std::string GeneralPatient::priorityLabel() const { return "Normal"; }
std::string GeneralPatient::detailLabel() const { return "Reason for visit"; }

}
