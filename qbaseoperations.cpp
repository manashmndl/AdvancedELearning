#include "qbaseoperations.h"
#include "ui_qbaseoperations.h"
#include "qbase.h"

/*
 * This is the controller class of QBase, most of the working function were used from QBase
 * */

QBaseOperations::QBaseOperations(QWidget *parent) :
    QWidget(parent), reserved_base_digits("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    ui(new Ui::QBaseOperations)
{
    ui->setupUi(this);
    //Hiding other groupboxes at beginning
    ui->binaryToGrayBCDGroupBox->setVisible(false);
    ui->complementGroupBox->setVisible(false);



    //Connecting custom spinbox with validator generator
    connect(ui->customBaseBox, SIGNAL(valueChanged(int)), this, SLOT(customBaseValidatorGenerator(int)));
    connect(ui->complementBaseSpinBox, SIGNAL(valueChanged(int)), this, SLOT(complementValidatorGenerator(int)));

    //Connecting float reducer with line edits
    connect(ui->binEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceBinEditFloat(QString)));
    connect(ui->hexEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceHexEditFloat(QString)));
    connect(ui->decEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceDecEditFloat(QString)));
    connect(ui->octEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceOctEditFloat(QString)));
    connect(ui->cusEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceCusEditFloat(QString)));
    connect(ui->rsInputNumberEdit, SIGNAL(textChanged(QString)), this, SLOT(reduceRsInputEditFloat(QString)));

    //Connecting conversion function with line edits
    connect(ui->binEdit, SIGNAL(textEdited(QString)), this, SLOT(binaryToOthers(QString)));
    connect(ui->decEdit, SIGNAL(textEdited(QString)), this, SLOT(decimalToOthers(QString)));
    connect(ui->cusEdit, SIGNAL(textEdited(QString)), this, SLOT(customToOthers(QString)));
    connect(ui->octEdit, SIGNAL(textEdited(QString)), this, SLOT(octalToOthers(QString)));
    connect(ui->hexEdit, SIGNAL(textEdited(QString)), this, SLOT(hexadecimalToOthers(QString)));
    connect(ui->grayLineEdit, SIGNAL(textEdited(QString)), this, SLOT(grayToBinary(QString)));
    connect(ui->binaryLineEdit, SIGNAL(textEdited(QString)), this, SLOT(binaryToGray(QString)));
    connect(ui->decNumberEdit, SIGNAL(textEdited(QString)), this , SLOT(BCD(QString)));

    /* Setting validators for
     * binEdit - Number Base Converter Group Box
     * hexEdit -            ""
     * decEdit -            ""
     * octEdit -            ""
     * grayLineEdit --  Binary To Gray and BCD Group Box
     * binaryLineEdit --        ""
     * decNumberEdit --         ""
     * */
    QRegExpValidator *binaryValidator = new QRegExpValidator(QRegExp("[01.]{40}"));
    ui->binEdit->setValidator(binaryValidator);
    QRegExpValidator *hexadecimalValidator = new QRegExpValidator(QRegExp("[0-9A-F.]{25}"));
    ui->hexEdit->setValidator(hexadecimalValidator);
    QRegExpValidator *decimalValidator = new QRegExpValidator(QRegExp("[0-9.]{40}"));
    ui->decEdit->setValidator(decimalValidator);
    QRegExpValidator *octalValidator = new QRegExpValidator(QRegExp("[0-7.]{40}"));
    ui->octEdit->setValidator(octalValidator);
    QRegExpValidator *binaryAndBCDValidator = new QRegExpValidator(QRegExp("[01]{40}"));
    ui->grayLineEdit->setValidator(binaryAndBCDValidator);
    ui->binaryLineEdit->setValidator(binaryAndBCDValidator);
    QRegExpValidator *decimalBCDValidator = new QRegExpValidator(QRegExp("[0-9]{40}"));
    ui->decNumberEdit->setValidator(decimalBCDValidator);


    //Initialize validators with current spinbox value
    complementValidatorGenerator(ui->complementBaseSpinBox->value());
    customBaseValidatorGenerator(ui->customBaseBox->value());

    //Connecting rs complement line edits with slots
    connect(ui->rsInputNumberEdit, SIGNAL(textEdited(QString)), this, SLOT(calculateRsComplement(QString)));


    //Keeping the layout fixed until the button is pressed
    layout()->setSizeConstraint(QLayout::SetFixedSize);


    //More button initialize with icons
    ui->moreButton->setText("More");
    ui->moreButton->setIcon(QIcon(":/images/left_arrow.svg"));
    ui->doneButton->setIcon(QIcon(":/images/done_icon.svg"));
    ui->clearButton->setIcon(QIcon(":/images/clear_icon.svg"));



    setWindowTitle("Number Base Operation Toolbox");

    setWindowIcon(QIcon(":/images/qbase_operation.svg"));


}

QBaseOperations::~QBaseOperations()
{
    delete ui;
}


//When more button is toggled it will hide or show the other group boxes based on it's status [C]
void QBaseOperations::on_moreButton_toggled(bool checked)
{
    if (checked){
        ui->binaryToGrayBCDGroupBox->setVisible(true);
        ui->complementGroupBox->setVisible(true);
        ui->moreButton->setText("Less");
        ui->moreButton->setIcon(QIcon(":/images/right_arrow.svg"));
    } else {
        ui->binaryToGrayBCDGroupBox->setVisible(false);
        ui->complementGroupBox->setVisible(false);
        ui->moreButton->setText("More");
        ui->moreButton->setIcon(QIcon(":/images/left_arrow.svg"));
    }
}

//Close the Widget when the button is clicked [C]
void QBaseOperations::on_doneButton_clicked()
{
    this->close();
}


//Custom validator generator for custom base lineedit [C]
void QBaseOperations::customBaseValidatorGenerator(int val)
{
    ui->cusEdit->clear();
    QString rx;
    for (int i = 0; i < val; i++){
        rx.append(reserved_base_digits.at(i));
    }
    rx = "[" + rx + ".]{40}";
    QRegExpValidator *customValidator = new QRegExpValidator(QRegExp(rx));
    ui->cusEdit->setValidator(customValidator);
}

//Custom validator generator for complement input editor [C]
void QBaseOperations::complementValidatorGenerator(int val)
{
    ui->rsInputNumberEdit->clear();
    QString rx;
    for (int i = 0; i < val; i++){
        rx.append(reserved_base_digits.at(i));
    }
    rx = "[" + rx + ".]{40}";
    QRegExpValidator *customValidator = new QRegExpValidator(QRegExp(rx));
    ui->rsInputNumberEdit->setValidator(customValidator);
}

//Clear all number base box line edits [C]
void QBaseOperations::clearBaseGBoxLineEdits()
{
    ui->decEdit->clear();
    ui->hexEdit->clear();
    ui->octEdit->clear();
    ui->binEdit->clear();
    ui->cusEdit->clear();
}


//Extra floating point reducer [C]
void QBaseOperations::removeExtraFloatingPoint(QString in, QLineEdit *currEdit)
{
    int pt_num = 0;
    for (int i = 0; i < in.size(); i++){
        if (in[i] == '.') pt_num++;
        if (pt_num > 1) {
            in = in.remove(i, 1);
            pt_num = 0;
            currEdit->setText(in);
        }
    }

}

//Binary Edit Extra float reducer [C]
void QBaseOperations::reduceBinEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->binEdit);
}

//Hexadecimal edit float reducer [C]
void QBaseOperations::reduceHexEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->hexEdit);
}

//Octal Edit float reducer [C]
void QBaseOperations::reduceOctEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->octEdit);
}

//Custom Edit float reducer [C]
void QBaseOperations::reduceCusEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->cusEdit);
}

//Decimal Edit float reducer [C]
void QBaseOperations::reduceDecEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->decEdit);
}

//Complement input edit float reducer [C]
void QBaseOperations::reduceRsInputEditFloat(QString in)
{
    removeExtraFloatingPoint(in, ui->rsInputNumberEdit);
}

//Binary to other conversion [C]
void QBaseOperations::binaryToOthers(QString in)
{
    QBase base;
    ui->hexEdit->setText(base.toOther(in, 2, 16));
    ui->decEdit->setText(base.toOther(in, 2, 10));
    ui->octEdit->setText(base.toOther(in, 2 , 8));
    ui->cusEdit->setText(base.toOther(in, 2, ui->customBaseBox->value()));
}

//Decimal to other conversion [C]
void QBaseOperations::decimalToOthers(QString in)
{
    QBase base;
    ui->binEdit->setText(base.toOther(in, 10, 2));
    ui->octEdit->setText(base.toOther(in, 10, 8));
    ui->hexEdit->setText(base.toOther(in, 10, 16));
    ui->cusEdit->setText(base.toOther(in, 10, ui->customBaseBox->value()));
}

//Custom base to others [C]
void QBaseOperations::customToOthers(QString in)
{
    QBase base;
    ui->binEdit->setText(base.toOther(in, ui->customBaseBox->value(), 2));
    ui->hexEdit->setText(base.toOther(in, ui->customBaseBox->value(), 16));
    ui->decEdit->setText(base.toOther(in, ui->customBaseBox->value(), 10));
    ui->octEdit->setText(base.toOther(in, ui->customBaseBox->value(), 8));

}

//Octal to others [C]
void QBaseOperations::octalToOthers(QString in)
{
    QBase base;
    ui->binEdit->setText(base.toOther(in, 8, 2));
    ui->hexEdit->setText(base.toOther(in, 8, 16));
    ui->decEdit->setText(base.toOther(in, 8, 10));
    ui->cusEdit->setText(base.toOther(in, 8, ui->customBaseBox->value()));
}

//Hexadecimal to others[C]
void QBaseOperations::hexadecimalToOthers(QString in)
{
    QBase base;
    ui->binEdit->setText(base.toOther(in, 16, 2));
    ui->decEdit->setText(base.toOther(in, 16, 10));
    ui->octEdit->setText(base.toOther(in, 16, 8));
    ui->cusEdit->setText(base.toOther(in, 16, ui->customBaseBox->value()));
}

//Clears line edits [C]
void QBaseOperations::on_clearButton_clicked()
{
    clearBaseGBoxLineEdits();
}

//Calculates r and r-1 complement and update the line edits with calculated results [C]
void QBaseOperations::calculateRsComplement(QString in)
{
    QBase base;

    qDebug() << in;

    if (!in.isEmpty()){

    ui->rMinusOneEdit->setText(base.calcRMinusOneComplement(ui->complementBaseSpinBox->value(), in));

    ui->rsComplementEdit->setText(base.calcRComplement(ui->complementBaseSpinBox->value(), in));

    }

    else{
        ui->rsComplementEdit->clear();
        ui->rMinusOneEdit->clear();
    }
}

//Clear the lineEdits on changing the spinBox value
void QBaseOperations::on_complementBaseSpinBox_valueChanged(int arg1)
{
    ui->rsComplementEdit->clear();
    ui->rMinusOneEdit->clear();
}

//Converts gray code to binary and updates
void QBaseOperations::grayToBinary(QString in)
{
    QBase base;
    ui->binaryLineEdit->setText(base.grayToBinary(in));
}

//duh!
void QBaseOperations::binaryToGray(QString in)
{
    QBase base;
    ui->grayLineEdit->setText(base.binaryToGray(in));
}

//Converts a decimal number into bcd
void QBaseOperations::BCD(QString in)
{
    QBase base;
    if (!in.isEmpty()) ui->BCDLineEdit->setText(base.toBCD(in)); //Extra care !!
    else ui->BCDLineEdit->setText(QString::null); //If they give empty they'll get empty
}

