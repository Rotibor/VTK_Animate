#ifndef _TIMER_CALLBACK_
#define _TIMER_CALLBACK_

#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"

class vtkTimerCallback2 : public vtkCommand
{
  public:
    static vtkTimerCallback2 *New()
    {
      vtkTimerCallback2 *cb = new vtkTimerCallback2;
      cb->TimerCount = 0;
      return cb;
    }
 
    virtual void Execute(vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData))
    {
      if (vtkCommand::TimerEvent == eventId)
        {
        ++this->TimerCount;
        }
      std::cout << this->TimerCount << std::endl;
      //actor->SetPosition(this->TimerCount, this->TimerCount,0);
	  actor->RotateX(2.0);
      vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
      iren->GetRenderWindow()->Render();
    }
 
  private:
    int TimerCount;
  public:
    vtkActor* actor;
};
#endif