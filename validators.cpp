#include "validators.h"
#include "cchatlib.h"

NameValidator::NameValidator() :
    QValidator(nullptr)
{

}

QValidator::State NameValidator::validate(QString &input, int &pos) const
{
    if (validate_name(input.toStdString().c_str()) < 0 || pos == MAX_NAME_LEN) {
        return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}

ContentValidator::ContentValidator() :
    QValidator(nullptr)
{

}

QValidator::State ContentValidator::validate(QString &input, int &pos) const
{
    if (validate_content(input.toStdString().c_str()) < 0 || pos == MAX_CONTENT_LEN) {
        return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}
