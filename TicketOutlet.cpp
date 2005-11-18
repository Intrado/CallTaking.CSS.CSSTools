#include "TicketOutlet.h"
#include "TicketPad.h"
#include <string>
#include "ToStr.h"
#include "cAutoLock.h"
#include "CSS.h"

namespace CSS
{  
  const char *cTicketOutlet::moduleName = "TicketOutlet";

  cTicketPad * cTicketOutlet::getTicketPad(string TicketPadName)
  {
    cTicketPad * TicketPadRef;
    {
      cAutoLock _lock(&lockTicketOutlet);
      TicketPadRef = NULL;
      CSSTrace(moduleName, "GetTicketPad", "Verify if TickedPad : ", TicketPadName, " exist ");;
      TicketPadRef = ((cTicketPad *) getInstance(TicketPadName));
      /*  Le TicketPad existe-t-il ? */
      if (TicketPadRef == NULL)
      {
        /*  Il n'existe pas, on en cree un */
        CSSTrace(moduleName, "GetTicketPad", "Create a TickedPad : ", TicketPadName);;
        TicketPadRef = (new cTicketPad)->init(TicketPadName);
        registerInstance(TicketPadName, TicketPadRef);
      }
      else
      {
        CSSTrace(moduleName, "GetTicketPad", "The TickedPad : ", TicketPadName, " exist");;
      };
      return TicketPadRef;
    };
  }
  
  /*  Lock */
  cTicketOutlet *cTicketOutlet::init()
  {
    CSSTrace(moduleName, "Init", "Enter...");;
    
    {
      cAutoLock _lock(&lockTicketOutlet);
      instanceList.clear();
    };
    CSSTrace(moduleName, "Init", "Exit...");;
    return this;
  }
  
  cTicketOutlet::~cTicketOutlet()
  {
    int size = instanceList.size();
    for (int i = 0; i < size; i++)
    {
      delete instanceList.back();
      instanceList.pop_back();
    };
  }
  
  bool cTicketOutlet::registerInstance(string NameInstance, cCSSObject * RefInstance)
  {
    int size;
    InstanceRec *rec;
    /*  Validation des arguments de passage */
    if (RefInstance == NULL)
    {
      return false;
    };
    if ((NameInstance).length() == 0)
    {
      return false;
    };
    size = instanceList.size();
    /*  Recherche si la classe de l'instance est deja enregistre */
    /*  Il est important de noter que la cle unique est la classe de l'instance 
    et non la reference de l'instance */
    if (size > 0)
    {
      for (int i = 0; i <= size - 1; i++)
      {
        rec = ((InstanceRec *) instanceList.at(i));
        if (rec->NameInstance == NameInstance)
        {
          CSSTrace(moduleName, "Init", "Name Instance Already Recorded Size: ", size);
          return true;
        };
      };
    };
    CSSTrace(moduleName, "Init", "Enregistrement");;
    rec = (new InstanceRec);
    rec->RefInstance = RefInstance;
    rec->NameInstance = NameInstance;
    instanceList.push_back(rec);
    return true;
  }
  
  bool cTicketOutlet::unregisterInstance(cCSSObject * RefInstance)
  {
    int size;
    /*  Ne sera problablement jamais utiliser mais elle est la */
    InstanceRec *rec;
    InstanceRec *rec2;
    if (RefInstance == NULL)
    {
      return false;
    };
    size = instanceList.size();
    if (size > 0)
    {
      for (int i = 0; i <= size - 1; i++)
      {
        rec = ((InstanceRec *) instanceList.at(i));
        if (rec->RefInstance == RefInstance)
        {
          CSSTrace(moduleName, "Unregister", "Unregistering ", rec->NameInstance);;
          if (i == size - 1)
          {
            CSSTrace(moduleName, "Init", "DESEnregistrement");;
            delete ((InstanceRec *)instanceList.back())->RefInstance;
            delete instanceList.back();
            instanceList.pop_back();
            break;
          }
          else if (i == 0)
          {
            CSSTrace(moduleName, "Init", "DESEnregistrement");;
            delete ((InstanceRec *)instanceList.back())->RefInstance;
            delete instanceList.front();
            instanceList.pop_front();
            break;
          }
          else
          {
            CSSTrace(moduleName, "Init", "DESEnregistrement");;
            rec2 = ((InstanceRec *) instanceList.back());
            
            delete ((InstanceRec *)instanceList.back())->RefInstance;
            delete instanceList.back();
            instanceList.pop_back();
            
            instanceList.erase(instanceList.begin() + i);
            instanceList.insert(instanceList.begin() + i, rec2);
            break;
          };
        };
      };
    };
    /*  FOR */
    return true;
  }
  
  cCSSObject *cTicketOutlet::getInstance(string Name)
  {
    int size;
    InstanceRec *rec;
    /* Verifie s'il l'instance demande possede une reference */
    size = instanceList.size();
    if (size > 0)
    {
      for (int i = 0; i <= size - 1; i++)
      {
        rec = ((InstanceRec *) instanceList.at(i));
        if ((rec->NameInstance == Name))
        {
          CSSTrace(moduleName, "GetInstance", "Reference trouvee : ", rec->NameInstance);;
          return rec->RefInstance;
        };
      };
    };
    return NULL;
  }
};
