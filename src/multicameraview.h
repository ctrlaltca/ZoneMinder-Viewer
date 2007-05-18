/***************************************************************************
 *   Copyright (C) 2007 by Leandro Emanuel López                           *
 *   lopezlean@gmail.com  				                   *
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
#ifndef MULTICAMERAVIEW_H
#define MULTICAMERAVIEW_H

#include <QWidget>
#include <QList>
class CameraWidget;
class Stream;

class MultiCameraView: public QWidget{
Q_OBJECT
public:
    MultiCameraView( QWidget * parent = 0 );
    void setStream ( QList <Stream * > stream  );
    ~MultiCameraView();
    void setUseTabs ( bool b );
    bool m_useTabs;
public Q_SLOTS:
    void close();
private:
    void init();
    void layoutCameras( QList <CameraWidget * > m_cameras  );
    void layoutCamerasWithTabs ( QList <CameraWidget * > m_cameras , int camerasPerPage = 4 );  
    //void createPage( );
    //void createPage( );
    //QList <CameraWidget * > *m_cameras;
    
};
#endif
