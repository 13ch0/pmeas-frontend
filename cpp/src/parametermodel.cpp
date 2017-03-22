#include "parametermodel.h"
#include "parameter.h"

#include <qqml.h>

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>

#include <QDir>


ParameterModel::ParameterModel(Effect::Type t_effectType, QObject *parent) : ParameterModel( parent ) {
    initializeParameters( t_effectType );
}

ParameterModel::ParameterModel( const QJsonArray &parameters, QObject *parent)  : ParameterModel( parent )
{
    beginInsertRows( QModelIndex(), m_model.size(), m_model.size() + parameters.size() );

    for ( const QJsonValue &val : parameters ) {
        QJsonObject parameterMap = val.toObject();

        m_model.append( Parameter{ parameterMap[ "name" ].toString()
                   , parameterMap[ "min" ].toInt(),
                    parameterMap[ "max" ].toInt()
                    , parameterMap[ "value"].toInt() } );
    }

    endInsertRows();
}

ParameterModel::ParameterModel(QObject *parent) : QAbstractTableModel( parent ),
    m_roleNames {
        { Roles::ParameterName, "parameterName" },
        { Roles::ParameterValue, "parameterValue" },
        { Roles::ParameterMinValue, "parameterMinValue" },
        { Roles::ParameterMaxValue, "parameterMaxValue" },
    }
{

}

// Attaches some column names to our model, so we can
// just call the column's name from QML and access
// the data.
QHash<int, QByteArray> ParameterModel::roleNames() const {
    return m_roleNames;
}

int ParameterModel::rowCount(const QModelIndex & ) const {
    return m_model.size();
}

int ParameterModel::columnCount(const QModelIndex & ) const {
    return m_roleNames.size();
}


// This is the getter function that QML calls when it wants to read our model.
QVariant ParameterModel::data(const QModelIndex &index, int role) const {

    if ( index.isValid() ) {

        switch( role ) {
            case Roles::ParameterName:
                return m_model[ index.row() ].name;
            case Roles::ParameterValue:
                return m_model[ index.row() ].value;
            case Roles::ParameterMinValue:
                return m_model[ index.row() ].min;
            case Roles::ParameterMaxValue:
                return m_model[ index.row() ].max;
            default:
                break;
        }
    }

    return QVariant();

}

// This is called whenever a parameter's row data is updated from QML.
bool ParameterModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if ( index.isValid() ) {

        switch( role ) {
            case Roles::ParameterName:
                m_model[ index.row() ].name = value.toString();
                break;
            case Roles::ParameterValue:
                m_model[ index.row() ].value = value.toInt();
                break;
            case Roles::ParameterMinValue:
                m_model[ index.row() ].min = value.toInt();
                break;
            case Roles::ParameterMaxValue:
                m_model[ index.row() ].max = value.toInt();
                break;
            default:
                return false;
        }

        emit dataChanged( index, index, { role });
        return true;
    }

    return false;
}

// Creates a header for our row. We will literally not need this.

QVariant ParameterModel::headerData(int section, Qt::Orientation orientation, int role) const  {

    (void)section;
    (void)orientation;

    switch( role ) {
        case Roles::ParameterName:
            return QVariant( "Name" );
        case Roles::ParameterValue:
            return QVariant( "Value" );
        case Roles::ParameterMinValue:
            return QVariant( "Minimum" );
        case Roles::ParameterMaxValue:
            return QVariant( "Maximum" );
        default:
            break;
    }

    return QVariant();

}

// Tells the view that we can edit, select, and enable rows in this model.
Qt::ItemFlags ParameterModel::flags(const QModelIndex &index) const {
    return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled ;
}

const Parameter &ParameterModel::at(int t_index) const {
    return m_model.at( t_index );
}

int ParameterModel::size() const {
    return m_model.size();
}

QVector<Parameter>::iterator ParameterModel::begin() {
    return m_model.begin();
}

QVector<Parameter>::const_iterator ParameterModel::cbegin() const {
    return m_model.cbegin();
}

QVector<Parameter>::iterator ParameterModel::end() {
    return m_model.end();
}

QVector<Parameter>::const_iterator ParameterModel::cend() const {
    return m_model.cend();
}


// Fills in the model with some default values. Nothing too crazy...
// Maybe subclass each parameter type if you *want* to.
void ParameterModel::initializeParameters( Effect::Type t_type ) {

    const int oldModelSize = m_model.size();

    switch( t_type ) {

        case Effect::Type::Invalid:
            break;

        case Effect::Type::Distortion:
            m_model.append( Parameter{ "Distortion", 0, 100, 50 } );
            m_model.append( Parameter{ "Tone", 0, 100, 50 } );
            break;

        case Effect::Type::Delay:
            m_model.append( Parameter{ "Feedback", 0, 100, 50 } );
            m_model.append( Parameter{ "Delay Time", 0, 100, 50 } );
            break;

        case Effect::Type::Reverb:
            m_model.append( Parameter{ "Effect Level", 0, 100, 50 } );
            m_model.append( Parameter{ "Tone", 0, 100, 50 } );
            m_model.append( Parameter{ "Delay", 0, 100, 50 } );
            m_model.append( Parameter{ "Room Size", 0, 100, 50 } );

            break;

        case Effect::Type::Harmonizer:
            m_model.append( Parameter{ "Balance", 0, 100, 50 } );
            m_model.append( Parameter{ "Shift", 0, 100, 50 } );
            break;

        case Effect::Type::FrequencyShift:
            m_model.append( Parameter{ "Balance", 0, 100, 50 } );
            m_model.append( Parameter{ "Pitch", 0, 100, 50 } );
            break;

        case Effect::Type::Phaser:
            m_model.append( Parameter{ "Distortion", 0, 100, 50 } );
            m_model.append( Parameter{ "Tone", 0, 100, 50 } );
            break;

        case Effect::Type::Chorus:
            m_model.append( Parameter{ "Effect Level", 0, 100, 50 } );
            m_model.append( Parameter{ "Rate", 0, 100, 50 } );
            m_model.append( Parameter{ "Depth", 0, 100, 50 } );
            break;

        case Effect::Type::Flanger:
            m_model.append( Parameter{ "Depth", 0, 100, 50 } );
            m_model.append( Parameter{ "Lfo Frequency", 0, 100, 50 } );
            m_model.append( Parameter{ "Feedback", 0, 100, 50 } );
            break;

        default:
            Q_UNREACHABLE();
    }

    // Tells the View that we have updated some rows.
    // The QML view will create some more rows for us graphically.
    //
    // These two functions HAVE to be called in order for the view
    // to know what we have changed.

    beginInsertRows( QModelIndex(), oldModelSize, m_model.size() );

    endInsertRows();
}

void ParameterModel::append( QString t_name, int t_min, int t_max, int t_value) {

    beginInsertRows( QModelIndex(), m_model.size(), m_model.size() );

    m_model.append( Parameter{ t_name , t_min, t_max, t_value } );

    endInsertRows();
}
