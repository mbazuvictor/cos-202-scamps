#include "model/EmergencyPatient.h"
#include "utils/Constants.h"

namespace scapms {

std::string EmergencyPatient::category() const { return constants::CATEGORY_EMERGENCY; }
std::string EmergencyPatient::describe() const { return "HIGH PRIORITY - attend immediately"; }
std::string EmergencyPatient::priorityLabel() const { return "High Priority"; }
std::string EmergencyPatient::detailLabel() const { return "Emergency contact / condition"; }

}
