#ifndef NAMEVALIDATOR_H
#define NAMEVALIDATOR_H

#include <QValidator>

class NameValidator : public QValidator
{
public:
    NameValidator();
    ~NameValidator() = default;

    QValidator::State validate(QString &input, int &pos) const;
};

class ContentValidator : public QValidator {
public:
    ContentValidator();
    ~ContentValidator() = default;

    QValidator::State validate(QString &input, int &pos) const;
};

#endif // NAMEVALIDATOR_H
