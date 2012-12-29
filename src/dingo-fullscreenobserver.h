/* 
 * dingo-fullscreenobserver.h
 *
 *  Copyright (C) 2011-2013 Phong V. Cao <phongvcao@phongvcao.com>
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 3 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#ifndef INCLUDED_DINGO_FULLSCREENOBSERVER_H
#define INCLUDED_DINGO_FULLSCREENOBSERVER_H

namespace Dingo {
  class FullscreenObserver {
    public:
      virtual void updateFullscreenActivated() = 0;
      virtual void updateUnFullscreenActivated() = 0;
  };
}

#endif //INCLUDED_DINGO_FULLSCREENOBSERVER_H
