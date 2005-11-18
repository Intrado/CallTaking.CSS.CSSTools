#ifndef __SimpleList_INCLUDED__
#define __SimpleList_INCLUDED__
#pragma warning(disable:4786)
#include <deque>
#include <string>
#include "CSSObject.h"
#include "cCritSec.h"
#include "cAutoLock.h"

template<class Elem> class SimpleList 
{
  class TNode: public cCSSObject 
  {
  public:
    TNode() 
    {
      item = NULL;
      next = NULL;
    };
    
    Elem *item;
    TNode *next;
  };
  
private:
  int objectCount;
  TNode *LookupNode;
  TNode *head;
  cCritSec *m;
  bool mIsInLookup;
  std::deque<Elem*> mToDeleteAfterLookup;
  friend class TNode;
  
public:
  SimpleList()
  {
    mIsInLookup = false;
    m = NULL;
  }
  
  ~SimpleList()
  {
    setLookup();
    TNode *tmp = lookupNext();
    while (tmp != NULL)
    {
      delete tmp;
      tmp = lookupNext();
    }
    endLookup();
  }
  
  
  SimpleList* init()        
  {
    m=(new cCritSec);
    LookupNode = NULL;
    objectCount=0;
    head = NULL;
    return this;
  }
  
  void add(cCSSObject *n1)
  {
    Elem *n = (Elem *)n1;
    TNode *tmpnode;
    cAutoLock _lock(m);
    
    objectCount++;
    if (objectCount>500) 
    {
      if (Diag::TraceOn) 
      {
        DiagTrace("SimpleList","Add","More than 500 Elements");;
      };
    };
    if (head==NULL) 
    {
      head=(new TNode);
      head->item=n;
      head->next=NULL;
    }
    else 
    {
      tmpnode=(new TNode);
      tmpnode->item=n;
      tmpnode->next=head;
      head=tmpnode;
    };
  }
  
  void remove(cCSSObject *o)
  {
    Elem *old = (Elem *)o;
    TNode *tmpnode;
    TNode *beforetmpnode;
    
    // If we are in lookup we wait for the end of the lookup to delete the elements
    if (mIsInLookup)
    {
      mToDeleteAfterLookup.push_back((Elem *)o);
      return;
    }
    else
    {                
      cAutoLock _lock(m);
      if (head==NULL) 
      {
        return;
      };
      tmpnode=head;
      beforetmpnode=NULL;
      while ((tmpnode!=NULL)&&(tmpnode->item!=old)) 
      {
        beforetmpnode=tmpnode;
        tmpnode=tmpnode->next;          
      };
      if ((tmpnode!=NULL)) 
      {
        objectCount--;
        if (objectCount<0) 
        {
          if (Diag::TraceOn) 
          {
            DiagError("SimpleList","Delete","Less than 0 elements");
          };
        };
        if (beforetmpnode==NULL) 
        {
          head=tmpnode->next;
          tmpnode->next=NULL;
          delete tmpnode;
        }
        else 
        {
          beforetmpnode->next=tmpnode->next;
          tmpnode->next=NULL;
          delete tmpnode;
        }
      }
    }
  }
  
  void setLookup()
  {        
    m->Acquire();
    mIsInLookup = true;
    LookupNode=head;
  }
  
  bool inLookup()
  {
    return mIsInLookup;
  }
  
  cCSSObject * lookupNext()
  {
    TNode *tmpnode;
    tmpnode=LookupNode;
    if (tmpnode!=NULL) 
    {
      LookupNode=LookupNode->next;
      return tmpnode->item;
    }
    else 
    {
      return NULL;
    };
  }
  
  void endLookup()
  {
    m->Release();
    mIsInLookup = false;
    
    for (unsigned int i = 0 ; i < mToDeleteAfterLookup.size() ; i++)
    {
      remove(mToDeleteAfterLookup.at(i));
    }
    mToDeleteAfterLookup.clear();
    m->Release();        
  }
};
  

  
#endif
