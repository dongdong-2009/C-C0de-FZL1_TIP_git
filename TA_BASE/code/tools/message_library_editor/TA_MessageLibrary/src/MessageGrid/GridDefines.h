#pragma once

const COLORREF CR_OBSOLETE = RGB(211, 211, 211); // grey for obsolete items
const COLORREF CR_NEW_ROW = RGB(170, 213, 255); // light blue for insert new item row
const COLORREF CR_INSERTED_ROW = RGB(221, 255, 221); // light blue for insert new item row

const COLORREF CLR_ERROR_LEVEL_1 = RGB(255, 255, 128); // Yellow for level 1 error
const COLORREF CLR_ERROR_LEVEL_2 = RGB(255, 206, 183); // Orange for level 2 error
const COLORREF CLR_ERROR_LEVEL_3 = RGB(255, 202, 202); // Red for level 3 error

const DWORD ROW_DATA_ADD =	0xFFFFFFFF; // Add item row (in the message list)

// the following are only used in the parameters list control
const DWORD ROW_DATA_DEL =  0x00000001; // The item has been deleted from the grid, but not the XML document
const DWORD ROW_DATA_NEW =	0x00000100; // The item is new and has not been saved to XML
const DWORD ROW_DATA_MOD =	0x00001000; // The item exists in the XML document, and has been modified
