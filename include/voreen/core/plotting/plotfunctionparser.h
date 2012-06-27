/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#ifndef VRN_PLOTFUNCTIONPARSER_H
#define VRN_PLOTFUNCTIONPARSER_H

#include "voreen/core/utils/GLSLparser/parser.h"

#include <list>

namespace voreen {

namespace glslparser {

class PlotFunctionLexer;

class PlotFunctionParser : public Parser {
public:
    PlotFunctionParser(PlotFunctionLexer* const lexer);

    PlotFunctionParser(std::istream* const is) throw (std::bad_alloc);

    PlotFunctionParser(const std::string& fileName) throw (std::bad_alloc);

    PlotFunctionParser(const std::list<Token*>& tokens);

    virtual ~PlotFunctionParser();

protected:
    virtual Token* nextToken();

    virtual void expandParseTree(const int productionID,
        const std::vector<Parser::ParserSymbol*>& reductionBody);

    // The following methods are from generated code.

    virtual ParserAction* action(const int state, const int realSymbolID) const;

    virtual ProductionStub* findProduction(const int productionID) const;

    virtual int gotoState(const int state, const int symbol) const;

    virtual std::string symbolID2String(const int symbolID) const;

private:

    std::list<Token*> tokens_;
};

}   // namespace glslparser

}   // namespace voreen

#endif