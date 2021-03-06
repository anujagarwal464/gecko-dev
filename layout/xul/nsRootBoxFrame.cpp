/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsHTMLParts.h"
#include "nsStyleConsts.h"
#include "nsGkAtoms.h"
#include "nsIPresShell.h"
#include "nsBoxFrame.h"
#include "nsStackLayout.h"
#include "nsIRootBox.h"
#include "nsIContent.h"
#include "nsXULTooltipListener.h"
#include "nsFrameManager.h"
#include "mozilla/BasicEvents.h"

using namespace mozilla;

// Interface IDs

//#define DEBUG_REFLOW

// static
nsIRootBox*
nsIRootBox::GetRootBox(nsIPresShell* aShell)
{
  if (!aShell) {
    return nullptr;
  }
  nsIFrame* rootFrame = aShell->FrameManager()->GetRootFrame();
  if (!rootFrame) {
    return nullptr;
  }

  if (rootFrame) {
    rootFrame = rootFrame->GetFirstPrincipalChild();
  }

  nsIRootBox* rootBox = do_QueryFrame(rootFrame);
  return rootBox;
}

class nsRootBoxFrame : public nsBoxFrame, public nsIRootBox {
public:

  friend nsIFrame* NS_NewBoxFrame(nsIPresShell* aPresShell, nsStyleContext* aContext);

  nsRootBoxFrame(nsIPresShell* aShell, nsStyleContext *aContext);

  NS_DECL_QUERYFRAME
  NS_DECL_FRAMEARENA_HELPERS

  virtual nsPopupSetFrame* GetPopupSetFrame() MOZ_OVERRIDE;
  virtual void SetPopupSetFrame(nsPopupSetFrame* aPopupSet) MOZ_OVERRIDE;
  virtual nsIContent* GetDefaultTooltip() MOZ_OVERRIDE;
  virtual void SetDefaultTooltip(nsIContent* aTooltip) MOZ_OVERRIDE;
  virtual nsresult AddTooltipSupport(nsIContent* aNode) MOZ_OVERRIDE;
  virtual nsresult RemoveTooltipSupport(nsIContent* aNode) MOZ_OVERRIDE;

  virtual nsresult AppendFrames(ChildListID     aListID,
                                nsFrameList&    aFrameList) MOZ_OVERRIDE;
  virtual nsresult InsertFrames(ChildListID     aListID,
                                nsIFrame*       aPrevFrame,
                                nsFrameList&    aFrameList) MOZ_OVERRIDE;
  virtual nsresult RemoveFrame(ChildListID     aListID,
                               nsIFrame*       aOldFrame) MOZ_OVERRIDE;

  virtual nsresult Reflow(nsPresContext*          aPresContext,
                          nsHTMLReflowMetrics&     aDesiredSize,
                          const nsHTMLReflowState& aReflowState,
                          nsReflowStatus&          aStatus) MOZ_OVERRIDE;
  virtual nsresult HandleEvent(nsPresContext* aPresContext,
                               WidgetGUIEvent* aEvent,
                               nsEventStatus* aEventStatus) MOZ_OVERRIDE;

  virtual void BuildDisplayList(nsDisplayListBuilder*   aBuilder,
                                const nsRect&           aDirtyRect,
                                const nsDisplayListSet& aLists) MOZ_OVERRIDE;

  /**
   * Get the "type" of the frame
   *
   * @see nsGkAtoms::rootFrame
   */
  virtual nsIAtom* GetType() const MOZ_OVERRIDE;

  virtual bool IsFrameOfType(uint32_t aFlags) const MOZ_OVERRIDE
  {
    // Override bogus IsFrameOfType in nsBoxFrame.
    if (aFlags & (nsIFrame::eReplacedContainsBlock | nsIFrame::eReplaced))
      return false;
    return nsBoxFrame::IsFrameOfType(aFlags);
  }
  
#ifdef DEBUG_FRAME_DUMP
  virtual nsresult GetFrameName(nsAString& aResult) const MOZ_OVERRIDE;
#endif

  nsPopupSetFrame* mPopupSetFrame;

protected:
  nsIContent* mDefaultTooltip;
};

//----------------------------------------------------------------------

nsIFrame*
NS_NewRootBoxFrame(nsIPresShell* aPresShell, nsStyleContext* aContext)
{
  return new (aPresShell) nsRootBoxFrame (aPresShell, aContext);
}

NS_IMPL_FRAMEARENA_HELPERS(nsRootBoxFrame)

nsRootBoxFrame::nsRootBoxFrame(nsIPresShell* aShell, nsStyleContext* aContext):
  nsBoxFrame(aShell, aContext, true)
{
  mPopupSetFrame = nullptr;

  nsCOMPtr<nsBoxLayout> layout;
  NS_NewStackLayout(aShell, layout);
  SetLayoutManager(layout);
}

nsresult
nsRootBoxFrame::AppendFrames(ChildListID     aListID,
                             nsFrameList&    aFrameList)
{
  nsresult  rv;

  NS_ASSERTION(aListID == kPrincipalList, "unexpected child list ID");
  NS_PRECONDITION(mFrames.IsEmpty(), "already have a child frame");
  if (aListID != kPrincipalList) {
    // We only support the principal child list.
    rv = NS_ERROR_INVALID_ARG;
  } else if (!mFrames.IsEmpty()) {
    // We only allow a single child frame.
    rv = NS_ERROR_FAILURE;
  } else {
    rv = nsBoxFrame::AppendFrames(aListID, aFrameList);
  }

  return rv;
}

nsresult
nsRootBoxFrame::InsertFrames(ChildListID     aListID,
                             nsIFrame*       aPrevFrame,
                             nsFrameList&    aFrameList)
{
  nsresult  rv;

  // Because we only support a single child frame inserting is the same
  // as appending.
  NS_PRECONDITION(!aPrevFrame, "unexpected previous sibling frame");
  if (aPrevFrame) {
    rv = NS_ERROR_UNEXPECTED;
  } else {
    rv = AppendFrames(aListID, aFrameList);
  }

  return rv;
}

nsresult
nsRootBoxFrame::RemoveFrame(ChildListID     aListID,
                            nsIFrame*       aOldFrame)
{
  nsresult  rv;

  NS_ASSERTION(aListID == kPrincipalList, "unexpected child list ID");
  if (aListID != kPrincipalList) {
    // We only support the principal child list.
    rv = NS_ERROR_INVALID_ARG;
  } else if (aOldFrame == mFrames.FirstChild()) {
    rv = nsBoxFrame::RemoveFrame(aListID, aOldFrame);
  } else {
    rv = NS_ERROR_FAILURE;
  }

  return rv;
}

#ifdef DEBUG_REFLOW
int32_t gReflows = 0;
#endif

nsresult
nsRootBoxFrame::Reflow(nsPresContext*           aPresContext,
                       nsHTMLReflowMetrics&     aDesiredSize,
                       const nsHTMLReflowState& aReflowState,
                       nsReflowStatus&          aStatus)
{
  DO_GLOBAL_REFLOW_COUNT("nsRootBoxFrame");

#ifdef DEBUG_REFLOW
  gReflows++;
  printf("----Reflow %d----\n", gReflows);
#endif
  return nsBoxFrame::Reflow(aPresContext, aDesiredSize, aReflowState, aStatus);
}

void
nsRootBoxFrame::BuildDisplayList(nsDisplayListBuilder*   aBuilder,
                                 const nsRect&           aDirtyRect,
                                 const nsDisplayListSet& aLists)
{
  // root boxes don't need a debug border/outline or a selection overlay...
  // They *may* have a background propagated to them, so force creation
  // of a background display list element.
  DisplayBorderBackgroundOutline(aBuilder, aLists, true);

  BuildDisplayListForChildren(aBuilder, aDirtyRect, aLists);
}

nsresult
nsRootBoxFrame::HandleEvent(nsPresContext* aPresContext,
                            WidgetGUIEvent* aEvent,
                            nsEventStatus* aEventStatus)
{
  NS_ENSURE_ARG_POINTER(aEventStatus);
  if (nsEventStatus_eConsumeNoDefault == *aEventStatus) {
    return NS_OK;
  }

  if (aEvent->message == NS_MOUSE_BUTTON_UP) {
    nsFrame::HandleEvent(aPresContext, aEvent, aEventStatus);
  }

  return NS_OK;
}

// REVIEW: The override here was doing nothing since nsBoxFrame is our
// parent class
nsIAtom*
nsRootBoxFrame::GetType() const
{
  return nsGkAtoms::rootFrame;
}

nsPopupSetFrame*
nsRootBoxFrame::GetPopupSetFrame()
{
  return mPopupSetFrame;
}

void
nsRootBoxFrame::SetPopupSetFrame(nsPopupSetFrame* aPopupSet)
{
  // Under normal conditions this should only be called once.  However,
  // if something triggers ReconstructDocElementHierarchy, we will
  // destroy this frame's child (the nsDocElementBoxFrame), but not this
  // frame.  This will cause the popupset to remove itself by calling
  // |SetPopupSetFrame(nullptr)|, and then we'll be able to accept a new
  // popupset.  Since the anonymous content is associated with the
  // nsDocElementBoxFrame, we'll get a new popupset when the new doc
  // element box frame is created.
  if (!mPopupSetFrame || !aPopupSet) {
    mPopupSetFrame = aPopupSet;
  } else {
    NS_NOTREACHED("Popup set is already defined! Only 1 allowed.");
  }
}

nsIContent*
nsRootBoxFrame::GetDefaultTooltip()
{
  return mDefaultTooltip;
}

void
nsRootBoxFrame::SetDefaultTooltip(nsIContent* aTooltip)
{
  mDefaultTooltip = aTooltip;
}

nsresult
nsRootBoxFrame::AddTooltipSupport(nsIContent* aNode)
{
  NS_ENSURE_TRUE(aNode, NS_ERROR_NULL_POINTER);

  nsXULTooltipListener *listener = nsXULTooltipListener::GetInstance();
  if (!listener)
    return NS_ERROR_OUT_OF_MEMORY;

  return listener->AddTooltipSupport(aNode);
}

nsresult
nsRootBoxFrame::RemoveTooltipSupport(nsIContent* aNode)
{
  // XXjh yuck, I'll have to implement a way to get at
  // the tooltip listener for a given node to make 
  // this work.  Not crucial, we aren't removing 
  // tooltips from any nodes in the app just yet.
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_QUERYFRAME_HEAD(nsRootBoxFrame)
  NS_QUERYFRAME_ENTRY(nsIRootBox)
NS_QUERYFRAME_TAIL_INHERITING(nsBoxFrame)

#ifdef DEBUG_FRAME_DUMP
nsresult
nsRootBoxFrame::GetFrameName(nsAString& aResult) const
{
  return MakeFrameName(NS_LITERAL_STRING("RootBox"), aResult);
}
#endif
