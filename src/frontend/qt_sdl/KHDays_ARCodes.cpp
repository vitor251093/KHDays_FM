/*
    Copyright 2016-2023 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include "KHDays_ARCodes.h"
#include <stdio.h>
#include <string.h>
#include "Platform.h"
#include "Screen.h"
#include "Config.h"
#include <math.h>

namespace melonDS
{
using namespace Platform;

KHDaysARCodes::KHDaysARCodes()
{
    Error = false;

    float aspectTop = (Config::WindowWidth * 1.f) / Config::WindowHeight;
    for (auto ratio : aspectRatios)
    {
        if (ratio.id == Config::ScreenAspectTop)
            aspectTop = ratio.ratio * 4.0/3;
    }
    if (aspectTop == 0) {
        aspectTop = 16.0 / 9;
    }
    ScreenAspect = aspectTop;

    Categories.clear();

    if (!Load())
        Error = true;
}

ARCode KHDaysARCodes::ChangeAspectRatio(std::string codeName, u32 address)
{
    // Also known as the "Widescreen hack"
    // TODO: Shouldn't be applied while the menu is opened

    // if (mem32[0x02023C9C] == 0x00001555) {
    //     mem32[0x02023C9C] = aspectRatioKey;
    // }

    int aspectRatioKey = (int)round(0x1000 * ScreenAspect);

    ARCode curcode;
    curcode.Name = codeName;
    curcode.Enabled = true;
    curcode.Code.clear();
    curcode.Code.push_back(0x50000000 | address); curcode.Code.push_back(0x00001555);
    curcode.Code.push_back(             address); curcode.Code.push_back(aspectRatioKey);
    curcode.Code.push_back(0xD2000000);           curcode.Code.push_back(0x00000000);
    return curcode;
}

ARCode KHDaysARCodes::AlwaysEnableXAndDPadToControlCommandMenu(std::string codeName, u32 address)
{
    // Example:
    // if (mem16[0x02194CC2] < 0x4300) {
    //     if (mem16[0x02194CC2] > 0x41FF) {
    //         mem8[0x02194CC3] = 0x40;
    //     }
    // }

    ARCode curcode2;
    curcode2.Name = codeName;
    curcode2.Enabled = true;
    curcode2.Code.clear();
    curcode2.Code.push_back((0x70000000 | address) - 0x1); curcode2.Code.push_back(0x4300);
    curcode2.Code.push_back((0x80000000 | address) - 0x1); curcode2.Code.push_back(0x41FF);
    curcode2.Code.push_back( 0x20000000 | address);        curcode2.Code.push_back(0x40);
    curcode2.Code.push_back( 0xD2000000);                  curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0xD2000000);                  curcode2.Code.push_back(0x00000000);
    return curcode2;
}

ARCode KHDaysARCodes::Enable360Movement(std::string codeName, u32 address)
{
    ARCode curcode2;
    curcode2.Name = codeName;
    curcode2.Enabled = true;
    curcode2.Code.clear();
    curcode2.Code.push_back( 0x537F84D8);        curcode2.Code.push_back(0xE12FFF1E);
    curcode2.Code.push_back( 0x037F8414);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0xE27F5CC4);        curcode2.Code.push_back(0x00000960);
    curcode2.Code.push_back( 0x9001B082);        curcode2.Code.push_back(0x3B019B01);
    curcode2.Code.push_back( 0x2B009301);        curcode2.Code.push_back(0xB002DCFA);
    curcode2.Code.push_back( 0xB5F04770);        curcode2.Code.push_back(0x9204B087);
    curcode2.Code.push_back( 0x93052209);        curcode2.Code.push_back(0x781BAB0C);
    curcode2.Code.push_back( 0x91039002);        curcode2.Code.push_back(0x2B009201);
    curcode2.Code.push_back( 0x2301D001);        curcode2.Code.push_back(0x23739301);
    curcode2.Code.push_back( 0x20024F31);        curcode2.Code.push_back(0xF7FF703B);
    curcode2.Code.push_back( 0x2377FFE3);        curcode2.Code.push_back(0x703B2608);
    curcode2.Code.push_back( 0xF7FF2002);        curcode2.Code.push_back(0x9B02FFDD);
    curcode2.Code.push_back( 0x2374781C);        curcode2.Code.push_back(0x432B09E5);
    curcode2.Code.push_back( 0x9801703B);        curcode2.Code.push_back(0xFFD4F7FF);
    curcode2.Code.push_back( 0x431D2376);        curcode2.Code.push_back(0x9801703D);
    curcode2.Code.push_back( 0x3E010664);        curcode2.Code.push_back(0xFFCCF7FF);
    curcode2.Code.push_back( 0x2E000E24);        curcode2.Code.push_back(0x9B03D1ED);
    curcode2.Code.push_back( 0xD1142B02);        curcode2.Code.push_back(0x785C9B02);
    curcode2.Code.push_back( 0x23743608);        curcode2.Code.push_back(0x432B09E5);
    curcode2.Code.push_back( 0x9801703B);        curcode2.Code.push_back(0xFFBCF7FF);
    curcode2.Code.push_back( 0x431D2376);        curcode2.Code.push_back(0x9801703D);
    curcode2.Code.push_back( 0x3E010664);        curcode2.Code.push_back(0xFFB4F7FF);
    curcode2.Code.push_back( 0x2E000E24);        curcode2.Code.push_back(0x9C04D1ED);
    curcode2.Code.push_back( 0x18E39B05);        curcode2.Code.push_back(0x9B029302);
    curcode2.Code.push_back( 0xD10B42A3);        curcode2.Code.push_back(0xF7FF2002);
    curcode2.Code.push_back( 0x2362FFA7);        curcode2.Code.push_back(0x703B2002);
    curcode2.Code.push_back( 0xFFA2F7FF);        curcode2.Code.push_back(0xBCF0B007);
    curcode2.Code.push_back( 0x4700BC01);        curcode2.Code.push_back(0x25002608);
    curcode2.Code.push_back( 0x703B2364);        curcode2.Code.push_back(0xF7FF9801);
    curcode2.Code.push_back( 0x2366FF97);        curcode2.Code.push_back(0x9801703B);
    curcode2.Code.push_back( 0xFF92F7FF);        curcode2.Code.push_back(0x783A2301);
    curcode2.Code.push_back( 0x0E2D066D);        curcode2.Code.push_back(0xD000421A);
    curcode2.Code.push_back( 0x3E01431D);        curcode2.Code.push_back(0xD1EB2E00);
    curcode2.Code.push_back( 0x34017025);        curcode2.Code.push_back(0x46C0E7D7);
    curcode2.Code.push_back( 0x04000138);        curcode2.Code.push_back(0x236EB51F);
    curcode2.Code.push_back( 0x7313466A);        curcode2.Code.push_back(0x22007350);
    curcode2.Code.push_back( 0x00132102);        curcode2.Code.push_back(0xA8039200);
    curcode2.Code.push_back( 0xFF7FF7FF);        curcode2.Code.push_back(0xBC01B005);
    curcode2.Code.push_back( 0x46C04700);        curcode2.Code.push_back(0xE92D401F);
    curcode2.Code.push_back( 0xE3A03065);        curcode2.Code.push_back(0xE5CD300F);
    curcode2.Code.push_back( 0xE3A03000);        curcode2.Code.push_back(0xE3A01001);
    curcode2.Code.push_back( 0xE58D3000);        curcode2.Code.push_back(0xE28D000F);
    curcode2.Code.push_back( 0xE3A03007);        curcode2.Code.push_back(0xE59F2068);
    curcode2.Code.push_back( 0xEB000122);        curcode2.Code.push_back(0xE59F1060);
    curcode2.Code.push_back( 0xE59F0060);        curcode2.Code.push_back(0xE5D13000);
    curcode2.Code.push_back( 0xE1A02203);        curcode2.Code.push_back(0xE1822223);
    curcode2.Code.push_back( 0xE20220FF);        curcode2.Code.push_back(0xE1A03122);
    curcode2.Code.push_back( 0xE1A02102);        curcode2.Code.push_back(0xE20220CC);
    curcode2.Code.push_back( 0xE2033033);        curcode2.Code.push_back(0xE1833002);
    curcode2.Code.push_back( 0xE1A020A3);        curcode2.Code.push_back(0xE1A03083);
    curcode2.Code.push_back( 0xE2022055);        curcode2.Code.push_back(0xE20330AA);
    curcode2.Code.push_back( 0xE1823003);        curcode2.Code.push_back(0xE4C13001);
    curcode2.Code.push_back( 0xE1510000);        curcode2.Code.push_back(0x1AFFFFEE);
    curcode2.Code.push_back( 0xE3A02000);        curcode2.Code.push_back(0xE3A00005);
    curcode2.Code.push_back( 0xE1A01002);        curcode2.Code.push_back(0xE59F3010);
    curcode2.Code.push_back( 0xE28DD014);        curcode2.Code.push_back(0xE49DE004);
    curcode2.Code.push_back( 0xE12FFF13);        curcode2.Code.push_back(0x027FFDE8);
    curcode2.Code.push_back( 0x027FFDEF);        curcode2.Code.push_back(0x037FE410);
    curcode2.Code.push_back( 0xB5102280);        curcode2.Code.push_back(0x4B054904);
    curcode2.Code.push_back( 0x88084C05);        curcode2.Code.push_back(0x800C601A);
    curcode2.Code.push_back( 0xBC10601A);        curcode2.Code.push_back(0x4708BC02);
    curcode2.Code.push_back( 0x04000134);        curcode2.Code.push_back(0x04000214);
    curcode2.Code.push_back( 0xFFFF8100);        curcode2.Code.push_back(0x4B022280);
    curcode2.Code.push_back( 0x4B02601A);        curcode2.Code.push_back(0x47708018);
    curcode2.Code.push_back( 0x04000214);        curcode2.Code.push_back(0x04000134);
    curcode2.Code.push_back( 0x200EB51F);        curcode2.Code.push_back(0x4468236D);
    curcode2.Code.push_back( 0x466A7003);        curcode2.Code.push_back(0x73D32300);
    curcode2.Code.push_back( 0x9300220F);        curcode2.Code.push_back(0x00193301);
    curcode2.Code.push_back( 0xF7FF446A);        curcode2.Code.push_back(0x466BFF02);
    curcode2.Code.push_back( 0xB0057BD8);        curcode2.Code.push_back(0x4708BC02);
    curcode2.Code.push_back( 0x2280B51F);        curcode2.Code.push_back(0x4B110004);
    curcode2.Code.push_back( 0x68194811);        curcode2.Code.push_back(0xD1044211);
    curcode2.Code.push_back( 0x28003801);        curcode2.Code.push_back(0x601AD1F9);
    curcode2.Code.push_back( 0x200EE013);        curcode2.Code.push_back(0x236F601A);
    curcode2.Code.push_back( 0x70034468);        curcode2.Code.push_back(0x2300466A);
    curcode2.Code.push_back( 0x220F73D3);        curcode2.Code.push_back(0x33019300);
    curcode2.Code.push_back( 0x446A0019);        curcode2.Code.push_back(0xFEE1F7FF);
    curcode2.Code.push_back( 0x7BD8466B);        curcode2.Code.push_back(0x42431A20);
    curcode2.Code.push_back( 0xB0044158);        curcode2.Code.push_back(0xBC02BC10);
    curcode2.Code.push_back( 0x46C04708);        curcode2.Code.push_back(0x04000214);
    curcode2.Code.push_back( 0x001F78A4);        curcode2.Code.push_back(0xF7FFB510);
    curcode2.Code.push_back( 0xBC10FF45);        curcode2.Code.push_back(0x4700BC01);
    curcode2.Code.push_back( 0x236CB51F);        curcode2.Code.push_back(0x7313466A);
    curcode2.Code.push_back( 0x22007351);        curcode2.Code.push_back(0x00130004);
    curcode2.Code.push_back( 0x92002102);        curcode2.Code.push_back(0xF7FFA803);
    curcode2.Code.push_back( 0x0020FEC0);        curcode2.Code.push_back(0xFF32F7FF);
    curcode2.Code.push_back( 0xBC01BC1F);        curcode2.Code.push_back(0xB5104700);
    curcode2.Code.push_back( 0xFF2CF7FF);        curcode2.Code.push_back(0xF7FF2080);
    curcode2.Code.push_back( 0xBC10FFB7);        curcode2.Code.push_back(0x4708BC02);
    curcode2.Code.push_back( 0xF7FFB510);        curcode2.Code.push_back(0x2080FFE1);
    curcode2.Code.push_back( 0xFFAEF7FF);        curcode2.Code.push_back(0xBC02BC10);
    curcode2.Code.push_back( 0xB5104708);        curcode2.Code.push_back(0xF7FF20FE);
    curcode2.Code.push_back( 0x2000FF19);        curcode2.Code.push_back(0xFFA4F7FF);
    curcode2.Code.push_back( 0xBC02BC10);        curcode2.Code.push_back(0xB5104708);
    curcode2.Code.push_back( 0xF7FF2082);        curcode2.Code.push_back(0xBC10FF0F);
    curcode2.Code.push_back( 0x4700BC01);        curcode2.Code.push_back(0x000CB570);
    curcode2.Code.push_back( 0x00050609);        curcode2.Code.push_back(0x20410E09);
    curcode2.Code.push_back( 0xFFDEF7FF);        curcode2.Code.push_back(0xD1032800);
    curcode2.Code.push_back( 0xBC702000);        curcode2.Code.push_back(0x4708BC02);
    curcode2.Code.push_back( 0x06090A21);        curcode2.Code.push_back(0x0E092081);
    curcode2.Code.push_back( 0xFFD2F7FF);        curcode2.Code.push_back(0xD0F32800);
    curcode2.Code.push_back( 0x06090C21);        curcode2.Code.push_back(0x0E092081);
    curcode2.Code.push_back( 0xFFCAF7FF);        curcode2.Code.push_back(0xD0EB2800);
    curcode2.Code.push_back( 0x20810E21);        curcode2.Code.push_back(0xFFC4F7FF);
    curcode2.Code.push_back( 0x2800192C);        curcode2.Code.push_back(0x42A5D0E4);
    curcode2.Code.push_back( 0x2083D10B);        curcode2.Code.push_back(0xFEE2F7FF);
    curcode2.Code.push_back( 0xF7FF2082);        curcode2.Code.push_back(0x2800FF6D);
    curcode2.Code.push_back( 0x2042D0DA);        curcode2.Code.push_back(0xFFABF7FF);
    curcode2.Code.push_back( 0x7829E7D7);        curcode2.Code.push_back(0xF7FF2081);
    curcode2.Code.push_back( 0x3501FFAF);        curcode2.Code.push_back(0xD1EA2800);
    curcode2.Code.push_back( 0xB510E7CE);        curcode2.Code.push_back(0x20440001);
    curcode2.Code.push_back( 0xFFA6F7FF);        curcode2.Code.push_back(0xBC02BC10);
    curcode2.Code.push_back( 0xB5F74708);        curcode2.Code.push_back(0x682B4D17);
    curcode2.Code.push_back( 0x23009301);        curcode2.Code.push_back(0x602B2780);
    curcode2.Code.push_back( 0xFF1EF7FF);        curcode2.Code.push_back(0x0004260A);
    curcode2.Code.push_back( 0xF7FF2001);        curcode2.Code.push_back(0x2082FEBB);
    curcode2.Code.push_back( 0xFF46F7FF);        curcode2.Code.push_back(0xD1072800);
    curcode2.Code.push_back( 0x2E003E01);        curcode2.Code.push_back(0x4B0ED1F4);
    curcode2.Code.push_back( 0x4B0E601F);        curcode2.Code.push_back(0xE7EB801C);
    curcode2.Code.push_back( 0x21DA260A);        curcode2.Code.push_back(0x0089480C);
    curcode2.Code.push_back( 0xFF9CF7FF);        curcode2.Code.push_back(0xD1032800);
    curcode2.Code.push_back( 0x2E003E01);        curcode2.Code.push_back(0xE7EED1F5);
    curcode2.Code.push_back( 0x4B052280);        curcode2.Code.push_back(0x4B05601A);
    curcode2.Code.push_back( 0x9B01801C);        curcode2.Code.push_back(0xBCF7602B);
    curcode2.Code.push_back( 0x4700BC01);        curcode2.Code.push_back(0x04000208);
    curcode2.Code.push_back( 0x04000214);        curcode2.Code.push_back(0x04000134);
    curcode2.Code.push_back( 0x027F62B8);        curcode2.Code.push_back(0x2300B5F8);
    curcode2.Code.push_back( 0x00074C0A);        curcode2.Code.push_back(0x60236826);
    curcode2.Code.push_back( 0xFEE6F7FF);        curcode2.Code.push_back(0x00050639);
    curcode2.Code.push_back( 0x20440E09);        curcode2.Code.push_back(0xFF40F7FF);
    curcode2.Code.push_back( 0x4B052280);        curcode2.Code.push_back(0x4B05601A);
    curcode2.Code.push_back( 0x6026801D);        curcode2.Code.push_back(0xBC01BCF8);
    curcode2.Code.push_back( 0x46C04700);        curcode2.Code.push_back(0x04000208);
    curcode2.Code.push_back( 0x04000214);        curcode2.Code.push_back(0x04000134);
    curcode2.Code.push_back( 0xE92D4030);        curcode2.Code.push_back(0xE59F417C);
    curcode2.Code.push_back( 0xE5943000);        curcode2.Code.push_back(0xE3530F41);
    curcode2.Code.push_back( 0xE24DD014);        curcode2.Code.push_back(0x0A000053);
    curcode2.Code.push_back( 0xCA000007);        curcode2.Code.push_back(0xE353000A);
    curcode2.Code.push_back( 0x0A000048);        curcode2.Code.push_back(0xE35300D2);
    curcode2.Code.push_back( 0x0A00004A);        curcode2.Code.push_back(0xE3530000);
    curcode2.Code.push_back( 0x0A00003D);        curcode2.Code.push_back(0xE2833001);
    curcode2.Code.push_back( 0xEA000044);        curcode2.Code.push_back(0xE59F2148);
    curcode2.Code.push_back( 0xE1530002);        curcode2.Code.push_back(0x0A00004B);
    curcode2.Code.push_back( 0xE3530D05);        curcode2.Code.push_back(0x1AFFFFF8);
    curcode2.Code.push_back( 0xE3A05001);        curcode2.Code.push_back(0xE3A0306B);
    curcode2.Code.push_back( 0xE1A01005);        curcode2.Code.push_back(0xE5CD300B);
    curcode2.Code.push_back( 0xE28D200C);        curcode2.Code.push_back(0xE3A03002);
    curcode2.Code.push_back( 0xE28D000B);        curcode2.Code.push_back(0xE58D5000);
    curcode2.Code.push_back( 0xEB000050);        curcode2.Code.push_back(0xE1DD20BC);
    curcode2.Code.push_back( 0xE1A03422);        curcode2.Code.push_back(0xE1833402);
    curcode2.Code.push_back( 0xE59F4108);        curcode2.Code.push_back(0xE1A03803);
    curcode2.Code.push_back( 0xE1A03823);        curcode2.Code.push_back(0xE14430BF);
    curcode2.Code.push_back( 0xE3A03071);        curcode2.Code.push_back(0xE1A01005);
    curcode2.Code.push_back( 0xE5CD300B);        curcode2.Code.push_back(0xE28D200C);
    curcode2.Code.push_back( 0xE3A03003);        curcode2.Code.push_back(0xE28D000B);
    curcode2.Code.push_back( 0xE58D5000);        curcode2.Code.push_back(0xEB000041);
    curcode2.Code.push_back( 0xE3A00EB5);        curcode2.Code.push_back(0xE1DD30DD);
    curcode2.Code.push_back( 0xE1DDC0DC);        curcode2.Code.push_back(0xE043200C);
    curcode2.Code.push_back( 0xE0020290);        curcode2.Code.push_back(0xE083300C);
    curcode2.Code.push_back( 0xE0030390);        curcode2.Code.push_back(0xE2822B02);
    curcode2.Code.push_back( 0xE1A02642);        curcode2.Code.push_back(0xE2833B02);
    curcode2.Code.push_back( 0xE2820007);        curcode2.Code.push_back(0xE1A03643);
    curcode2.Code.push_back( 0xE350000E);        curcode2.Code.push_back(0xE2830007);
    curcode2.Code.push_back( 0x93A02000);        curcode2.Code.push_back(0xE350000E);
    curcode2.Code.push_back( 0x93A03000);        curcode2.Code.push_back(0xE5DD100E);
    curcode2.Code.push_back( 0xE2622000);        curcode2.Code.push_back(0xE2633000);
    curcode2.Code.push_back( 0xE1A02802);        curcode2.Code.push_back(0xE1A03803);
    curcode2.Code.push_back( 0xE2011006);        curcode2.Code.push_back(0xE1A02822);
    curcode2.Code.push_back( 0xE1A03823);        curcode2.Code.push_back(0xE544100B);
    curcode2.Code.push_back( 0xE14420B9);        curcode2.Code.push_back(0xE14430B7);
    curcode2.Code.push_back( 0xE28DD014);        curcode2.Code.push_back(0xE8BD4030);
    curcode2.Code.push_back( 0xE12FFF1E);        curcode2.Code.push_back(0xE59F3060);
    curcode2.Code.push_back( 0xE3A00005);        curcode2.Code.push_back(0xE59F105C);
    curcode2.Code.push_back( 0xE1A0E00F);        curcode2.Code.push_back(0xE12FFF13);
    curcode2.Code.push_back( 0xE5943000);        curcode2.Code.push_back(0xEAFFFFBA);
    curcode2.Code.push_back( 0xEB00001D);        curcode2.Code.push_back(0xE3A0300B);
    curcode2.Code.push_back( 0xE5843000);        curcode2.Code.push_back(0xEAFFFFF1);
    curcode2.Code.push_back( 0xE3A00001);        curcode2.Code.push_back(0xEB000012);
    curcode2.Code.push_back( 0xE3A030D3);        curcode2.Code.push_back(0xEAFFFFF9);
    curcode2.Code.push_back( 0xE3A00002);        curcode2.Code.push_back(0xEB00000E);
    curcode2.Code.push_back( 0xE59F3024);        curcode2.Code.push_back(0xEAFFFFF5);
    curcode2.Code.push_back( 0xE3A00003);        curcode2.Code.push_back(0xEB00000A);
    curcode2.Code.push_back( 0xE59F3018);        curcode2.Code.push_back(0xEAFFFFF1);
    curcode2.Code.push_back( 0x027F6280);        curcode2.Code.push_back(0x00000136);
    curcode2.Code.push_back( 0x027FFDFF);        curcode2.Code.push_back(0x037FE39C);
    curcode2.Code.push_back( 0x027F5DE0);        curcode2.Code.push_back(0x00000105);
    curcode2.Code.push_back( 0x00000137);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x00000000);        curcode2.Code.push_back(0xE59FC000);
    curcode2.Code.push_back( 0xE12FFF1C);        curcode2.Code.push_back(0x027F60A1);
    curcode2.Code.push_back( 0xE59FC000);        curcode2.Code.push_back(0xE12FFF1C);
    curcode2.Code.push_back( 0x027F5CD7);        curcode2.Code.push_back(0xE59FC000);
    curcode2.Code.push_back( 0xE12FFF1C);        curcode2.Code.push_back(0x027F602F);
    curcode2.Code.push_back( 0x00000000);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x00000000);        curcode2.Code.push_back(0xE92D5FFE);
    curcode2.Code.push_back( 0xEB000001);        curcode2.Code.push_back(0xE8BD9FFE);
    curcode2.Code.push_back( 0x00000000);        curcode2.Code.push_back(0xE2403001);
    curcode2.Code.push_back( 0xE3530002);        curcode2.Code.push_back(0x8A000003);
    curcode2.Code.push_back( 0xE92D4010);        curcode2.Code.push_back(0xEB0000A0);
    curcode2.Code.push_back( 0xE3A00000);        curcode2.Code.push_back(0xE8BD8010);
    curcode2.Code.push_back( 0xE3A00000);        curcode2.Code.push_back(0xE12FFF1E);
    curcode2.Code.push_back( 0x4B04B513);        curcode2.Code.push_back(0x681C9100);
    curcode2.Code.push_back( 0x00012300);        curcode2.Code.push_back(0x0018220E);
    curcode2.Code.push_back( 0xBD1647A0);        curcode2.Code.push_back(0x00201368);
    curcode2.Code.push_back( 0xB083B5F0);        curcode2.Code.push_back(0x98080004);
    curcode2.Code.push_back( 0x9B091A1D);        curcode2.Code.push_back(0x3B012000);
    curcode2.Code.push_back( 0x29004013);        curcode2.Code.push_back(0x2B00D003);
    curcode2.Code.push_back( 0xB003D014);        curcode2.Code.push_back(0x2B00BDF0);
    curcode2.Code.push_back( 0x1811D1FB);        curcode2.Code.push_back(0x5CE65CCF);
    curcode2.Code.push_back( 0xD00542B7);        curcode2.Code.push_back(0x30012300);
    curcode2.Code.push_back( 0xD1F642A8);        curcode2.Code.push_back(0xE7F02000);
    curcode2.Code.push_back( 0x33019E08);        curcode2.Code.push_back(0xD1F7429E);
    curcode2.Code.push_back( 0xE7EA0008);        curcode2.Code.push_back(0x27002600);
    curcode2.Code.push_back( 0x5D9819D3);        curcode2.Code.push_back(0x5DA39301);
    curcode2.Code.push_back( 0x5D8B4058);        curcode2.Code.push_back(0xD1054398);
    curcode2.Code.push_back( 0x36019B08);        curcode2.Code.push_back(0xD10342B3);
    curcode2.Code.push_back( 0xE7DA9801);        curcode2.Code.push_back(0x37012600);
    curcode2.Code.push_back( 0xD1ED42AF);        curcode2.Code.push_back(0xE7D42000);
    curcode2.Code.push_back( 0xB5F72310);        curcode2.Code.push_back(0x22802702);
    curcode2.Code.push_back( 0x23A09300);        curcode2.Code.push_back(0x21004D0F);
    curcode2.Code.push_back( 0x97010028);        curcode2.Code.push_back(0x0352029B);
    curcode2.Code.push_back( 0xFFBAF7FF);        curcode2.Code.push_back(0xD0132800);
    curcode2.Code.push_back( 0x230E2401);        curcode2.Code.push_back(0x43204E0A);
    curcode2.Code.push_back( 0x22806030);        curcode2.Code.push_back(0x93000028);
    curcode2.Code.push_back( 0x210023A0);        curcode2.Code.push_back(0x97013010);
    curcode2.Code.push_back( 0x0352029B);        curcode2.Code.push_back(0xFFA8F7FF);
    curcode2.Code.push_back( 0xD0012800);        curcode2.Code.push_back(0x60744304);
    curcode2.Code.push_back( 0x46C0BDF7);        curcode2.Code.push_back(0x002012B8);
    curcode2.Code.push_back( 0x00201368);        curcode2.Code.push_back(0x4D12B57F);
    curcode2.Code.push_back( 0x2B00682B);        curcode2.Code.push_back(0x2602D01F);
    curcode2.Code.push_back( 0x23A0240E);        curcode2.Code.push_back(0x21002280);
    curcode2.Code.push_back( 0x9601480E);        curcode2.Code.push_back(0x029B9400);
    curcode2.Code.push_back( 0xF7FF0352);        curcode2.Code.push_back(0x2800FF8D);
    curcode2.Code.push_back( 0x6B43D011);        curcode2.Code.push_back(0x001A0021);
    curcode2.Code.push_back( 0x70163246);        curcode2.Code.push_back(0x33472254);
    curcode2.Code.push_back( 0x230F701A);        curcode2.Code.push_back(0x446B32A8);
    curcode2.Code.push_back( 0x3AFB701A);        curcode2.Code.push_back(0x22009200);
    curcode2.Code.push_back( 0x0010686D);        curcode2.Code.push_back(0xBD7F47A8);
    curcode2.Code.push_back( 0x00201368);        curcode2.Code.push_back(0x002012D6);
    curcode2.Code.push_back( 0xB5072302);        curcode2.Code.push_back(0x330A9301);
    curcode2.Code.push_back( 0x93002280);        curcode2.Code.push_back(0x210023A0);
    curcode2.Code.push_back( 0x029B4804);        curcode2.Code.push_back(0xF7FF0352);
    curcode2.Code.push_back( 0x2800FF67);        curcode2.Code.push_back(0x6E00D001);
    curcode2.Code.push_back( 0xBD0E300C);        curcode2.Code.push_back(0x002012E4);
    curcode2.Code.push_back( 0xB573230E);        curcode2.Code.push_back(0x93002280);
    curcode2.Code.push_back( 0x23A02602);        curcode2.Code.push_back(0x48142100);
    curcode2.Code.push_back( 0x029B9601);        curcode2.Code.push_back(0xF7FF0352);
    curcode2.Code.push_back( 0x4C12FF53);        curcode2.Code.push_back(0x60A04D12);
    curcode2.Code.push_back( 0xD1012800);        curcode2.Code.push_back(0xBD762000);
    curcode2.Code.push_back( 0x00282308);        curcode2.Code.push_back(0x22809300);
    curcode2.Code.push_back( 0x210023A0);        curcode2.Code.push_back(0x96013046);
    curcode2.Code.push_back( 0x0352029B);        curcode2.Code.push_back(0xFF40F7FF);
    curcode2.Code.push_back( 0x280060E0);        curcode2.Code.push_back(0xF7FFD0EE);
    curcode2.Code.push_back( 0x6120FFC7);        curcode2.Code.push_back(0xD0E92800);
    curcode2.Code.push_back( 0x60232300);        curcode2.Code.push_back(0xFF6CF7FF);
    curcode2.Code.push_back( 0xFF94F7FF);        curcode2.Code.push_back(0xE7E22001);
    curcode2.Code.push_back( 0x002012F0);        curcode2.Code.push_back(0x00201368);
    curcode2.Code.push_back( 0x002012B8);        curcode2.Code.push_back(0x466BB573);
    curcode2.Code.push_back( 0x33074C26);        curcode2.Code.push_back(0x70187822);
    curcode2.Code.push_back( 0xD03E2A00);        curcode2.Code.push_back(0x2A01781A);
    curcode2.Code.push_back( 0xF7FFD123);        curcode2.Code.push_back(0x7020FFBD);
    curcode2.Code.push_back( 0xD0362800);        curcode2.Code.push_back(0x68EB4D20);
    curcode2.Code.push_back( 0xD002079A);        curcode2.Code.push_back(0x801A2200);
    curcode2.Code.push_back( 0x22003302);        curcode2.Code.push_back(0x5D103401);
    curcode2.Code.push_back( 0x32011899);        curcode2.Code.push_back(0x2A5C7008);
    curcode2.Code.push_back( 0x692AD1F9);        curcode2.Code.push_back(0x655A6829);
    curcode2.Code.push_back( 0x430A2201);        curcode2.Code.push_back(0x1E9C659A);
    curcode2.Code.push_back( 0x1C202660);        curcode2.Code.push_back(0xDF7A1C31);
    curcode2.Code.push_back( 0x68AC4B14);        curcode2.Code.push_back(0x80232502);
    curcode2.Code.push_back( 0x781AE014);        curcode2.Code.push_back(0xD1152A02);
    curcode2.Code.push_back( 0x250422F0);        curcode2.Code.push_back(0x02124B0E);
    curcode2.Code.push_back( 0x68DB6899);        curcode2.Code.push_back(0x3B041C8C);
    curcode2.Code.push_back( 0x02581B1B);        curcode2.Code.push_back(0x43100D40);
    curcode2.Code.push_back( 0x085B4A0B);        curcode2.Code.push_back(0xB29B4313);
    curcode2.Code.push_back( 0x808B8048);        curcode2.Code.push_back(0x1C291C20);
    curcode2.Code.push_back( 0xBD73DF7A);        curcode2.Code.push_back(0x2B03781B);
    curcode2.Code.push_back( 0x4B03D1FB);        curcode2.Code.push_back(0x2394689C);
    curcode2.Code.push_back( 0xE7DB019B);        curcode2.Code.push_back(0x00201308);
    curcode2.Code.push_back( 0x00201368);        curcode2.Code.push_back(0xFFFFE06C);
    curcode2.Code.push_back( 0xFFFFF800);        curcode2.Code.push_back(0xE59FC000);
    curcode2.Code.push_back( 0xE12FFF1C);        curcode2.Code.push_back(0x002011F9);
    curcode2.Code.push_back( 0x00000000);        curcode2.Code.push_back(0xB081B5FF);
    curcode2.Code.push_back( 0x460D4614);        curcode2.Code.push_back(0x9F0A461E);
    curcode2.Code.push_back( 0x46119801);        curcode2.Code.push_back(0xB081B5FF);
    curcode2.Code.push_back( 0x46054614);        curcode2.Code.push_back(0x9F0A461E);
    curcode2.Code.push_back( 0xB5709902);        curcode2.Code.push_back(0x4C0B4606);
    curcode2.Code.push_back( 0x180D0088);        curcode2.Code.push_back(0x5D614630);
    curcode2.Code.push_back( 0x43080E01);        curcode2.Code.push_back(0x403443B0);
    curcode2.Code.push_back( 0xB2844320);        curcode2.Code.push_back(0x1DC02500);
    curcode2.Code.push_back( 0x2D005745);        curcode2.Code.push_back(0x4938D008);
    curcode2.Code.push_back( 0xB5F36926);        curcode2.Code.push_back(0xB0AD1E04);
    curcode2.Code.push_back( 0xFFFFDA3C);        curcode2.Code.push_back(0x07250001);
    curcode2.Code.push_back( 0xFF574530);        curcode2.Code.push_back(0x07466EB5);
    curcode2.Code.push_back( 0x0E230024);        curcode2.Code.push_back(0x0C466922);
    curcode2.Code.push_back( 0x8D43A139);        curcode2.Code.push_back(0x0FB41046);
    curcode2.Code.push_back( 0x00280D48);        curcode2.Code.push_back(0x0B4780D4);
    curcode2.Code.push_back( 0x104A0B4C);        curcode2.Code.push_back(0x09789178);
    curcode2.Code.push_back( 0x00430802);        curcode2.Code.push_back(0x6A636002);
    curcode2.Code.push_back( 0x90795146);        curcode2.Code.push_back(0x000A0068);
    curcode2.Code.push_back( 0x20430802);        curcode2.Code.push_back(0x80203364);
    curcode2.Code.push_back( 0x01882101);        curcode2.Code.push_back(0xB5802143);
    curcode2.Code.push_back( 0xC0BDFF46);        curcode2.Code.push_back(0xC4710046);
    curcode2.Code.push_back( 0x0000001E);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0xFFFFFF00);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x037F84D8);        curcode2.Code.push_back(0xEABFF6FF);
    curcode2.Code.push_back( 0xD0000000);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x427FFC3C);        curcode2.Code.push_back(0x00000300);
    curcode2.Code.push_back( 0x62000A78);        curcode2.Code.push_back(0xE92D403E);
    curcode2.Code.push_back( 0xE2000A78);        curcode2.Code.push_back(0x000000B0);
    curcode2.Code.push_back( 0xE92D403E);        curcode2.Code.push_back(0xE59F409C);
    curcode2.Code.push_back( 0xE1D440B0);        curcode2.Code.push_back(0xE3540000);
    curcode2.Code.push_back( 0x1A000000);        curcode2.Code.push_back(0xE8BD803E);
    curcode2.Code.push_back( 0xE1A05804);        curcode2.Code.push_back(0xE1A04C04);
    curcode2.Code.push_back( 0xE1A04C44);        curcode2.Code.push_back(0xE1A05C45);
    curcode2.Code.push_back( 0xE2655000);        curcode2.Code.push_back(0xE1A00004);
    curcode2.Code.push_back( 0xE1A01005);        curcode2.Code.push_back(0xE59F5070);
    curcode2.Code.push_back( 0xE12FFF35);        curcode2.Code.push_back(0xE2800902);
    curcode2.Code.push_back( 0xE8BD803E);        curcode2.Code.push_back(0xE92D0007);
    curcode2.Code.push_back( 0xE59F0058);        curcode2.Code.push_back(0xE1D020F6);
    curcode2.Code.push_back( 0xE1B02282);        curcode2.Code.push_back(0xC28EE078);
    curcode2.Code.push_back( 0xB28EE024);        curcode2.Code.push_back(0xB2622000);
    curcode2.Code.push_back( 0x158620A8);        curcode2.Code.push_back(0x13A07010);
    curcode2.Code.push_back( 0xE8BD0007);        curcode2.Code.push_back(0xE2175004);
    curcode2.Code.push_back( 0xE12FFF1E);        curcode2.Code.push_back(0xE92D0007);
    curcode2.Code.push_back( 0xE59F0028);        curcode2.Code.push_back(0xE1D020F8);
    curcode2.Code.push_back( 0xE1B02202);        curcode2.Code.push_back(0xC28EE024);
    curcode2.Code.push_back( 0xB28EE088);        curcode2.Code.push_back(0xB2622000);
    curcode2.Code.push_back( 0x158620AC);        curcode2.Code.push_back(0x13A07010);
    curcode2.Code.push_back( 0x13A00000);        curcode2.Code.push_back(0xE8BD0007);
    curcode2.Code.push_back( 0xE2172001);        curcode2.Code.push_back(0xE12FFF1E);
    curcode2.Code.push_back( 0x027FFDF0);        curcode2.Code.push_back(0x0200524C);
    curcode2.Code.push_back( 0xD2000000);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x427FFC3C);        curcode2.Code.push_back(0x00000400);
    curcode2.Code.push_back( 0x520A084C);        curcode2.Code.push_back(0xE12FFF1E);
    curcode2.Code.push_back( 0x020A084C);        curcode2.Code.push_back(0xEAFD8089);
    curcode2.Code.push_back( 0xD2000000);        curcode2.Code.push_back(0x00000000);
    curcode2.Code.push_back( 0x427FFC3C);        curcode2.Code.push_back(0x00000400);
    curcode2.Code.push_back( 0x5204DE40);        curcode2.Code.push_back(0xE2172001);
    curcode2.Code.push_back( 0x0204DD60);        curcode2.Code.push_back(0xEBFECB55);
    curcode2.Code.push_back( 0x0204DE40);        curcode2.Code.push_back(0xEBFECB29);
    curcode2.Code.push_back( 0xD2000000);        curcode2.Code.push_back(0x00000000);
    return curcode2;
}

bool KHDaysARCodes::Load()
{
    // References
    // https://uk.codejunkies.com/support_downloads/Trainer-Toolkit-for-Nintendo-DS-User-Manual.pdf

    Categories.clear();

    ARCodeCat curcat;
    curcat.Name = "KHDaysCheats";
    curcat.Codes.clear();

    curcat.Codes.push_back(ChangeAspectRatio("Auto Resolution (US)", 0x02023C9C));
    curcat.Codes.push_back(ChangeAspectRatio("Auto Resolution (EU)", 0x02023CBC));
    curcat.Codes.push_back(AlwaysEnableXAndDPadToControlCommandMenu("Always X + D-Pad (US)",      0x02194CC3));
    curcat.Codes.push_back(AlwaysEnableXAndDPadToControlCommandMenu("Always X + D-Pad (EU)",      0x02195AA3));
    curcat.Codes.push_back(AlwaysEnableXAndDPadToControlCommandMenu("Always X + D-Pad (JP)",      0x02193E23));
    curcat.Codes.push_back(AlwaysEnableXAndDPadToControlCommandMenu("Always X + D-Pad (JP Rev1)", 0x02193DA3));
    //curcat.Codes.push_back(Enable360Movement("360 Movement (US)", 0));

    Categories.push_back(curcat);
    return true;
}

bool KHDaysARCodes::Save()
{
    return false;
}

}