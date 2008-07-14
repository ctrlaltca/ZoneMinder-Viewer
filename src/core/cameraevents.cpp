/***************************************************************************
 *   Copyright (C) 2007 by Leandro Emanuel López                           *
 *   lopezlean@gmail.com                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cameraevents.h"
#include "camerawidget.h"
#include "camerawidgettoolbar.h"
#include "framewidget.h"
#include "stream.h"


#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QCalendarWidget>
#include <QTextCharFormat>
#include <QDate>
#include <QPushButton>

class CameraEventCalendar : public QCalendarWidget{
public:
    CameraEventCalendar( QWidget * parent ): QCalendarWidget ( parent ){};
    void appendDate( const QDate & date ){
        QTextCharFormat fmt;
        fmt.setBackground( QBrush( Qt::yellow ) );
        fmt.setToolTip(tr("Click here to view events of this day"));
        fmt.setFontUnderline( true );
        setDateTextFormat( date , fmt );
        m_eventsDateList.append( date );
    }

    QList <QDate> eventsDateList() const{
        return m_eventsDateList;
    }
private:
    QList <QDate> m_eventsDateList;

};



CameraEvents::CameraEvents ( int camId, const QString & connectionName , QWidget * parent )
        :QDialog ( parent )
{
    m_cameraId = camId;
    m_connectionName = connectionName;
    init();
}

void CameraEvents::init()
{
    QVBoxLayout * layout = new QVBoxLayout ( this );
    setLayout( layout );

    QHBoxLayout * hLayout = new QHBoxLayout;
    QVBoxLayout * calendarLayout = new QVBoxLayout;
    m_camera = new CameraWidget( NULL , this );
    m_camera->setCameraType( CameraWidget::EventViewer );
    m_camera->toolBar()->setVisible( false );
    m_camera->setFixedSize( 640, 480 );
    m_camera->frameWidget()->setErrorMessage( tr("Please select an event") );

    m_calendarWidget = new CameraEventCalendar( this );
    m_calendarWidget->setGridVisible( true );
    m_clearFilterButton = new QPushButton( tr("Clear Calendar Filters"), this  );
    m_clearFilterButton->setEnabled( false );
    connect ( m_calendarWidget , SIGNAL( clicked ( QDate ) ), this , SLOT ( filterEventDate( QDate ) ) );
    connect ( m_clearFilterButton , SIGNAL( clicked (  ) ), this , SLOT ( restoreFilter(  ) ) );

    m_model = new QSqlTableModel ( this , QSqlDatabase::database ( m_connectionName ) );
    m_model->setTable ( "Events" );
    m_model->setFilter ( "MonitorId = " + QString::number ( m_cameraId ) );
    m_model->setEditStrategy ( QSqlTableModel::OnManualSubmit );
    m_model->select();

    for ( int i = 0 ; i < m_model->rowCount(); i++ ){
        m_calendarWidget->appendDate (m_model->record( i ).value( "StartTime" ).toDate());
    }

    m_model->setHeaderData( Name, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData( Cause, Qt::Horizontal, tr("Cause"));
    m_model->setHeaderData( StartTime, Qt::Horizontal, tr("Start"));
    m_model->setHeaderData( EndTime, Qt::Horizontal, tr("End"));
    m_model->setHeaderData( Length, Qt::Horizontal, tr("Length"));
    m_model->setHeaderData( Frames, Qt::Horizontal, tr("Frames"));
    m_model->setHeaderData( AlarmFrames, Qt::Horizontal, tr("Alarm Frames"));
    m_model->setHeaderData( Notes, Qt::Horizontal, tr("Notes"));

    m_view = new QTableView ( this );
    m_view->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    m_view->setSelectionBehavior ( QAbstractItemView::SelectRows );
    m_view->setAlternatingRowColors ( true );
    m_view->setSortingEnabled ( true );
    m_view->setModel ( m_model );

    QLabel * calendarLabel = new QLabel( tr("<center><h2>Events Calendar</h2></center>"
    "<p>The days that have events are marked yellow</p>"
    ), this );
    calendarLayout->addSpacerItem( new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    calendarLayout->addWidget( calendarLabel );
    calendarLayout->addWidget( m_calendarWidget );
    calendarLayout->addWidget( m_clearFilterButton );

    hLayout->addWidget( m_camera );
    hLayout->addLayout( calendarLayout );
    layout->addLayout( hLayout );
    layout->setAlignment( m_camera, Qt::AlignCenter );
    layout->addWidget ( m_view );

    connect ( m_view , SIGNAL ( clicked ( QModelIndex ) ), this , SLOT ( showEvent ( QModelIndex ) ) );

    m_view->setColumnHidden ( Id, true );
    m_view->setColumnHidden ( MonitorId, true );
    m_view->setColumnHidden ( Width, true );
    m_view->setColumnHidden ( Height, true );
    m_view->setColumnHidden ( TotScore, true );
    m_view->setColumnHidden ( MaxScore, true );
    m_view->setColumnHidden ( AvgScore, true );
    m_view->setColumnHidden ( Archivied, true );
    m_view->setColumnHidden ( Videoed, true );
    m_view->setColumnHidden ( Uploaded, true );
    m_view->setColumnHidden ( Emailed, true );
    m_view->setColumnHidden ( Messaged, true );
    m_view->setColumnHidden ( Executed, true );
    m_view->setColumnHidden ( LearnState, true );
    m_view->resizeColumnsToContents();
}

void CameraEvents::appendZMSString( const QString & s ){
    m_appendString = s;
}

CameraWidget * CameraEvents::cameraWidget( ) const{
    return m_camera;
}

void CameraEvents::showEvent ( const QModelIndex & index )
{
    m_camera->stopCamera();
    int eventId = m_model->data ( m_model->index ( index.row(), Id ) ).toInt();
    QSqlDatabase db = QSqlDatabase::database ( m_connectionName );
    QSqlQuery query = db.exec ( "SELECT Value from Config where Name='ZM_PATH_ZMS'" );
    query.next();
    QString zms = query.value ( 0 ).toString();
    query.clear();
    query = db.exec ( "SELECT * from Monitors where Id = " + m_model->data ( m_model->index ( index.row(), 1 ) ).toString() );
    query.next();

    m_camera->setWindowTitle ( m_model->data ( m_model->index ( index.row(), Name ) ).toString() );
    m_camera->stream()->setHost ( db.hostName() ,query.value ( query.record().indexOf ( "Port" ) ).toInt() );
    m_camera->stream()->setStreamType ( Stream::Event );
    m_camera->stream()->setEvent ( eventId );
    m_camera->stream()->setZMStreamServer ( zms );
    m_camera->stream()->appendZMSString( m_appendString );
    m_camera->setAutoAdjustImage ( true );
    m_camera->startCamera();
    query.clear();
    m_camera->show();
}


void CameraEvents::filterEventDate( const QDate & date ){
    if ( m_calendarWidget->eventsDateList().contains( date ) ){
        m_clearFilterButton->setEnabled( true );
        m_model->setFilter( "MonitorId = " + QString::number ( m_cameraId ) + " AND " + "date(StartTime) = '" + date.toString( Qt::ISODate ) + "'" );
        m_model->select();
    }
}
void CameraEvents::restoreFilter( ){
        m_clearFilterButton->setEnabled( false );
        m_model->setFilter( "MonitorId = " + QString::number ( m_cameraId ) );
        m_model->select();
}

void CameraEvents::resizeEvent ( QResizeEvent * event ){
    QWidget::resizeEvent( event );
}

CameraEvents::~CameraEvents()
{
}

#include "cameraevents.moc"

