#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkPolygon.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRegularPolygonSource.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>
#include <vtkCommand.h> 
#include <vtkSliderWidget.h>
#include <vtkSphereSource.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkImageTracerWidget.h>
#include <vtkAnimationScene.h>

#include "TimerCallback.h"
#include "Animation.h"

#include <iostream>

using namespace std;

//http://www.vtk.org/Wiki/VTK/Examples/Cxx

/*
the base VTK class:

	
	vtkObjectBase* obj = vtkExampleClass::New(); // use reference counting
	otherObject->SetExample(obj);
	obj->Delete();

	better way:
	vtkSmartPointer<vtkObjectBase> obj = vtkSmartPointer<vtkExampleClass>::New(); //no delete needed

	//data types
	vtkImageData
	vtkRectilinearGrid
	vtkStructuredGrid
	vtkPolyData
	vtkUnstructuredGrid
*/
//***************************************************************
// Define interaction style
class MyInteractorStyle : public vtkInteractorStyleTrackballActor
{
  public:
    static MyInteractorStyle* New();
    vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballActor);
 
    virtual void OnLeftButtonDown() 
    {
      std::cout << "Pressed left mouse button." << std::endl;
 
      vtkSmartPointer<vtkMatrix4x4> m = 
          vtkSmartPointer<vtkMatrix4x4>::New();
      this->Actor->GetMatrix(m);
      std::cout << "Matrix: " << endl << *m << std::endl;
 
      // Forward events
      vtkInteractorStyleTrackballActor::OnLeftButtonDown();
    }
 
    virtual void OnLeftButtonUp() 
    {
      std::cout << "Released left mouse button." << std::endl;
 
      vtkSmartPointer<vtkMatrix4x4> m = 
          vtkSmartPointer<vtkMatrix4x4>::New();
      this->Actor->GetMatrix(m);
      std::cout << "Matrix: " << endl << *m << std::endl;
 
      // Forward events
      vtkInteractorStyleTrackballActor::OnLeftButtonUp();
    }
 
    void SetActor(vtkSmartPointer<vtkActor> actor) {this->Actor = actor;}
 
  private:
    vtkSmartPointer<vtkActor> Actor;
 
 
};
vtkStandardNewMacro(MyInteractorStyle);
//***************************************************************
// The callback does the work.
// The callback keeps a pointer to the sphere whose resolution is
// controlled. After constructing the callback, the program sets the
// SphereSource of the callback to 
// the object to be controlled.
class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback *New() 
    {
    return new vtkSliderCallback;
    }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
    vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    this->SphereSource->SetPhiResolution(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue()/2);
    this->SphereSource->SetThetaResolution(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    }
  vtkSliderCallback():SphereSource(0) {}
  vtkSphereSource *SphereSource;
};
//***************************************************************

//***************************************************************
int main(int, char *argv[])
{
  /*
	 create a data source (cylinder)
	 any data that can be visualized
  */  

  // Inputs
  double p1[3] = {0.1, 0, -1.0};
  double p2[3] = {0.1, 0, 1.0};
  double tolerance = 0.001;
  // Outputs
  double t; // Parametric coordinate of intersection (0 (corresponding to p1) to 1 (corresponding to p2))
  double x[3]; // The coordinate of the intersection
  double pcoords[3];
  int subId;
 
  //-------------------------------------------------------
  // A sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(4.0);
  sphereSource->SetPhiResolution(4);
  sphereSource->SetThetaResolution(8);

  vtkSmartPointer<vtkPolyDataMapper> mapperSphere = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperSphere->SetInputConnection(sphereSource->GetOutputPort());
 
  vtkSmartPointer<vtkActor> actorSphere = vtkSmartPointer<vtkActor>::New();
  actorSphere->SetMapper(mapperSphere);
  actorSphere->GetProperty()->SetInterpolationToFlat();
  //-------------------------------------------------------
  vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
  cylinderSource->SetCenter(0.0, 0.0, 0.0);
  cylinderSource->SetRadius(5.0);
  cylinderSource->SetHeight(7.0);
  cylinderSource->SetResolution(100);
 
  vtkSmartPointer<vtkPolyDataMapper> mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cylinderSource->GetOutputPort());
  //-------------------------------------------------------
  // Create a pentagon (used regular polygone)
  vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
 
  //polygonSource->GeneratePolygonOff();
  polygonSource->SetNumberOfSides(12);
  polygonSource->SetRadius(1);  
  polygonSource->SetCenter(0,0,0);
  polygonSource->Update();
  //-------------------------------------------------------
  vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
  lineSource->SetPoint1(p1);
  lineSource->SetPoint2(p2);
  lineSource->Update();
  //--------------------------------------------------------
  // Create a square in the XY plane
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);
  // Create the polygon
  vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
  polygon->GetPoints()->DeepCopy(points);
  polygon->GetPointIds()->SetNumberOfIds(4); // 4 corners of the square
  polygon->GetPointIds()->SetId(0, 0);
  polygon->GetPointIds()->SetId(1, 1);
  polygon->GetPointIds()->SetId(2, 2);
  polygon->GetPointIds()->SetId(3, 3);

  vtkSmartPointer<vtkUnstructuredGrid> polyGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
  polyGrid->Allocate(1, 1);
  polyGrid->InsertNextCell(polygon->GetCellType(), polygon->GetPointIds());
  polyGrid->SetPoints(points);
  //--------------------------------------------------------

// Add the polygon to a list of polygons
  vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
  polygons->InsertNextCell(polygon);
 
  // Create a PolyData
  vtkSmartPointer<vtkPolyData> polygonPolyData = vtkSmartPointer<vtkPolyData>::New();
  polygonPolyData->SetPoints(points);
  polygonPolyData->SetPolys(polygons);
 

  vtkIdType iD = polygon->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId);
 
  std::cout << "intersected? " << iD << std::endl;
  std::cout << "intersection: " << x[0] << " " << x[1] << " " << x[2] << std::endl;




  /*
	create data filter
  */
  
  /*
	 Create mapper - geometric representation of actor
  */


  vtkSmartPointer<vtkPolyDataMapper> polyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  polyMapper->SetInputConnection(polygonSource->GetOutputPort());
  
  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper1->SetInput(polygonPolyData);
#else
  mapper1->SetInputData(polygonPolyData);
#endif
  //----------------------------------------------
  //  line mapper
  vtkSmartPointer<vtkPolyDataMapper> lineMapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
  lineMapper->SetInputConnection(lineSource->GetOutputPort());
  //---------------------------------------------


  /*
	Create an actor. Actor represents an object rendered in the scene, along with its properties
	and position. It can be treated as logical entity in the scene
  */
  vtkSmartPointer<vtkActor> actor      = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkActor> polyActor  = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkActor> polyActor1 = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkActor> lineActor  = vtkSmartPointer<vtkActor>::New();

  actor->SetMapper(mapper);
  polyActor->SetMapper(polyMapper);   
  polyActor1->SetMapper(mapper1);   
  lineActor->SetMapper(lineMapper);   
  lineActor->GetProperty()->SetLineWidth(4);
  

  //vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
 // actor->SetMapper(polyMapper);

  /*	
	A renderer coordinates the the rendering process involving lights, cameras and actors
  */
  vtkSmartPointer<vtkRenderer>     renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("test");
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);


  // Add the actor to the scene
  //renderer->AddActor(actor);
  //renderer->AddActor(polyActor);
  //renderer->AddActor(polyActor1);
  //renderer->AddActor(lineActor);

  renderer->AddActor(actorSphere);//@@@@@@
  renderer->SetBackground(.1, .3,.2); // Background color dark green
 
  //*****************************************
  //*****************************************

	  vtkSmartPointer<vtkSliderRepresentation3D> sliderRep = vtkSmartPointer<vtkSliderRepresentation3D>::New();
	  sliderRep->SetMinimumValue(3.0);
	  sliderRep->SetMaximumValue(50.0);
	  sliderRep->SetValue(sphereSource->GetThetaResolution());
	  sliderRep->SetTitleText("Sphere Resolution");
	  sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
	  sliderRep->GetPoint1Coordinate()->SetValue(-4,6,0);
	  sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
	  sliderRep->GetPoint2Coordinate()->SetValue(4,6,0);
	  sliderRep->SetSliderLength(0.075);
	  sliderRep->SetSliderWidth(0.05);
	  sliderRep->SetEndCapLength(0.05);
 
	  vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
	  sliderWidget->SetInteractor(renderWindowInteractor);
	  sliderWidget->SetRepresentation(sliderRep);
	  sliderWidget->SetAnimationModeToAnimate();
	  sliderWidget->EnabledOn();
 
	  vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	  callback->SphereSource = sphereSource;

	  sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);


	  //vtkSmartPointer<vtkImageTracerWidget> tracerWidget = vtkSmartPointer<vtkImageTracerWidget>::New();
	  //tracerWidget->SetInteractor(renderWindowInteractor);
	  //tracerWidget->SetViewProp(actor);
	  //tracerWidget->On();
  //*****************************************

  //----------------- animation --------------
  // Render and interact  


		  // Create an Animation Scene
		  vtkSmartPointer<vtkAnimationScene> scene = vtkSmartPointer<vtkAnimationScene>::New();
		  scene->SetModeToRealTime();
		  //scene->SetModeToSequence();
		  scene->SetLoop(0);
		  scene->SetFrameRate(10);
		  scene->SetStartTime(0);
		  scene->SetEndTime(5);

		  vtkSmartPointer<AnimationSceneObserver> sceneObserver = vtkSmartPointer<AnimationSceneObserver>::New();
		  sceneObserver->SetRenderWindow(renderWindow);
		  scene->AddObserver(vtkCommand::AnimationCueTickEvent,sceneObserver);
 
		  // Create an Animation Cue for each actor
		  vtkSmartPointer<vtkAnimationCue> cue1 = vtkSmartPointer<vtkAnimationCue>::New();
		  cue1->SetStartTime(0);
		  cue1->SetEndTime(5);
		  scene->AddCue(cue1);

		  // Create an ActorAnimator for each actor;
		  std::vector<double> endPos(3),  startPos(3);
		  endPos[0]   = -1; endPos[1]   = -1; endPos[2]   = -1;
		  startPos[0] = 10;  startPos[1] = 1;  startPos[2] = 1;

		  ActorAnimator animateSphere;
		  animateSphere.SetActor(actorSphere);
		  animateSphere.SetStartPosition(startPos);
		  animateSphere.SetEndPosition(endPos);
		  animateSphere.AddObserversToCue(cue1);


		  
		  //renWin->Render();
		  renderer->ResetCamera();
		  //renderer->GetActiveCamera()->Dolly(.5);		  
		  //renderer->ResetCameraClippingRange();		  
		  renderWindow->Render();
		  renderWindowInteractor->Initialize();		  

  //-----------------------------------------

  /*
  std::vector<double> endCone(3);
    endCone[0] = -1;
    endCone[1] = -1;
    endCone[2] = -1;
  animateSphere.SetEndPosition(endCone);
  animateSphere.SetActor(cone);
  animateSphere.AddObserversToCue(cue2);
  
  */
  // Create Cue observer.
  scene->Play();
  scene->Stop();
  

  /*
	 // Sign up to receive TimerEvent
	  vtkSmartPointer<vtkTimerCallback2> cb = vtkSmartPointer<vtkTimerCallback2>::New();
	  cb->actor = actorSphere;
	  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);
 
	  int timerId = renderWindowInteractor->CreateRepeatingTimer(100);
	  std::cout << "timerId: " << timerId << std::endl;
  */
  

  renderWindowInteractor->Start();  
  return EXIT_SUCCESS;
}
