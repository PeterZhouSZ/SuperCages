#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "GUI/glCanvas.h"
#include "GUI/rigPanel.h"
#include "GUI/characterPanel.h"
#include "GUI/cagePanel.h"
#include "GUI/skeletonPanel.h"
#include "GUI/asyncAnimatorPanel.h"
#include "GUI/toolsPanel.h"
#include "common/animatorOperations.h"

#include "qsplitter.h"
#include "GUI/restPoseCanvas.h"

#include "GUI/qtUtils.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent),
     ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   controller = Controller::get();

   controller->mainWindow = this;

   //DEBUG VIEW
   QSplitter *hSplit = new QSplitter(Qt::Horizontal);


   //configure glCanvas
   controller->glCanvas = new GlCanvas(hSplit);
   controller->glCanvas->setObjectName(QStringLiteral("GlCanvas"));


   //configure debugCanvas
   controller->restPoseCanvas = new RestPoseCanvas(hSplit);
   controller->restPoseCanvas->setObjectName(QStringLiteral("DebugCanvas"));

   // Set main QSplitter as the main widget.
   ui->verticalLayout->addWidget(hSplit);
   //hSplit->show();


   //configure fbxImportPanel
   if(!controller->rigPanel)
   {
      controller->rigPanel = new RigPanel(this);
   }
   controller->rigPanel->show();
   addDockWidget(Qt::LeftDockWidgetArea, controller->rigPanel);

   //configure trimeshPanel
   if(!controller->characterPanel)
   {
      controller->characterPanel = new CharacterPanel(this);
   }
   controller->characterPanel->show();
   addDockWidget(Qt::LeftDockWidgetArea, controller->characterPanel);


   //configure CagePanel
   if(!controller->cagePanel)
   {
      controller->cagePanel = new CagePanel(this);
   }
   controller->cagePanel->show();
   addDockWidget(Qt::LeftDockWidgetArea, controller->cagePanel);


   //configure SkeletonPanel
   if(!controller->skeletonPanel)
   {
      controller->skeletonPanel = new SkeletonPanel(this);
   }
   controller->skeletonPanel->show();
   addDockWidget(Qt::LeftDockWidgetArea, controller->skeletonPanel);

   //configure AsyncAnimatorPanel
   if(!controller->asyncAnimatorPanel)
   {
      controller->asyncAnimatorPanel = new AsyncAnimatorPanel(this);
   }
   controller->asyncAnimatorPanel->show();
   addDockWidget(Qt::LeftDockWidgetArea, controller->asyncAnimatorPanel);

   //configure ToolPanel
   if(!controller->toolsPanel)
   {
      controller->toolsPanel = new ToolsPanel(this);
   }
   controller->toolsPanel->show();
   addDockWidget(Qt::RightDockWidgetArea, controller->toolsPanel);

   updateGUI();

   /*tabifyDockWidget(controller->rigPanel, controller->characterPanel);*/
   tabifyDockWidget(controller->characterPanel, controller->cagePanel);
   tabifyDockWidget(controller->cagePanel, controller->skeletonPanel);
   controller->characterPanel->raise();

   showMaximized();
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_actionSmallHumanoid_triggered()
{
   //quickLoadFBX("/quickload/smallhumanoid.fbx");
   //quickLoadSkelAnimation("/quickload/smallhumanoid_skelAnim.txt");
   //quickLoadCageAnimation("/quickload/smallhumanoid_cageAnim.txt");
   //controller->asyncAnimatorPanel->updateKeyframeList();
}

void MainWindow::on_actionHorse_triggered()
{
   //quickLoadFBX("/quickload/horse.fbx");
}
