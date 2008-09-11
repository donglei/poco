//
// LayoutRenderer.cpp
//
// $Id: //poco/Main/WebWidgets/ExtJS/src/LayoutRenderer.cpp#5 $
//
// Library: ExtJS
// Package: Core
// Module:  LayoutRenderer
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/WebWidgets/ExtJS/LayoutRenderer.h"
#include "Poco/WebWidgets/ExtJS/FormRenderer.h"
#include "Poco/WebWidgets/ExtJS/Utility.h"
#include "Poco/WebWidgets/Layout.h"
#include "Poco/WebWidgets/VerticalLayout.h"
#include "Poco/WebWidgets/Frame.h"
#include "Poco/WebWidgets/Panel.h"
#include <sstream>


namespace Poco {
namespace WebWidgets {
namespace ExtJS {


LayoutRenderer::LayoutRenderer()
{
}


LayoutRenderer::~LayoutRenderer()
{
}


void LayoutRenderer::renderLayoutHead(const Layout* pLayout, const RenderContext& context, std::ostream& ostr, const std::string& layoutId, const std::string& layoutConfig, int cols, int horPad, int vertPad)
{
	poco_assert_dbg(pLayout != 0);
	Renderable::ID id(0);
	View::Ptr ptrParent = pLayout->parent();
	if (!(ptrParent && (ptrParent.cast<Frame>() || ptrParent.cast<Panel>())))
	{
		// the parent is not a panel 
		// assume that the direct parent is a panel
		ostr << "new Ext.Panel({border:false,bodyBorder:false,";
		ostr << "id:'" << pLayout->id() << "',";
		if (!pLayout->isVisible())
			ostr << "hidden:true,";
		renderParameters(pLayout, context, ostr, layoutId, layoutConfig, cols, horPad, vertPad);
		ostr << "})";
	}
	else
	{
		renderParameters(pLayout, context, ostr, layoutId, layoutConfig, cols, horPad, vertPad);
	}
}



void LayoutRenderer::renderBody(const Renderable* pRenderable, const RenderContext& context, std::ostream& ostr)
{
}


void LayoutRenderer::renderParameters(const Layout* pLayout, const RenderContext& context, std::ostream& ostr, const std::string& layoutId, const std::string& layoutConfig, int cols, int horPad, int vertPad)
{
	poco_assert_dbg(pLayout != 0);
	bool writeComma = false;
	if (pLayout->getWidth() > 0)
		ostr << "width:" << pLayout->getWidth() << ",";
	if (!layoutId.empty())
	{
		ostr << "layout:'" << layoutId << "'";
		writeComma = true;
	}
	if (!layoutConfig.empty())
	{
		if (writeComma)
			ostr << ",layoutConfig:" << layoutConfig;
		else
			ostr << ",layoutConfig:" << layoutConfig;
		writeComma = true;
	}
	if (writeComma)
		ostr << ",items:[";
	else
		ostr << "items:[";
	visitChildren(pLayout, cols, horPad, vertPad, context, ostr);
	ostr << "]";
}


void LayoutRenderer::visitChildren(const Layout* pLayout, int cols, int horPad, int vertPad, const RenderContext& context, std::ostream& ostr)
{
	std::string padHor;
	
	if (horPad > 0)
	{
		std::ostringstream pad;
		pad << "<p class=\"lbl\" style=\"margin-left:" << (horPad-4) << "px\">&nbsp;</p>"; // -4 fixes size of &nbsp;
		padHor = pad.str();
	}
	
	std::string padVert;
	if (vertPad > 0)
	{
		std::ostringstream pad;
		pad << "<p style=\"margin-top:" << vertPad << "px\"></p>";
		padVert = pad.str();
	}
	ContainerView::ConstIterator it = pLayout->begin();
	int cnt(0);	
	for (; it != pLayout->end(); ++it, ++cnt)
	{
		if (cnt > 0)
		{
			ostr << ",";
			if (cnt < cols && !padHor.empty())
			{
				ostr << "new Ext.Panel({border:false,bodyBorder:false,";
				ostr << "html:'" << padHor << "'}),";
				++cnt;
			}
			else
			{
				if (cnt >= cols)
				{
					if (cnt % cols == 0) //first row
					{
						if (!padVert.empty())
						{
							//insert a complete line!
							for (int i= 0; i < cols; ++i)
							{
								ostr << "new Ext.Panel({border:false,bodyBorder:false,";
								ostr << "html:'" << padVert << "'}),";
							}
						} // else no hor padding for first row!
					}
					else if (!padHor.empty())
					{
						ostr << "new Ext.Panel({border:false,bodyBorder:false,";
						ostr << "html:'" << padHor << "'}),";
						++cnt;
					}
				}
			}
		}
		if (*it)
		{
			//horizontallayout works only when children are panels
			ostr << "new Ext.Panel({border:false,bodyBorder:false,items:[";
			(*it)->renderHead(context, ostr);
			ostr << "]})";
		}
		else
			ostr << "{}";
		
	}
}


} } } // namespace Poco::WebWidgets::ExtJS