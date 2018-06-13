"Resource/UI/HudDeathWeaponDesire.res"
{	
	"HintLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"HintLabel"
		"xpos"			"0"
		"ypos"			"-10"
		"zpos"			"2"
		"wide"			"150"
		"tall"			"35"
		"visible"		"1"
		"enabled"		"1"
		"textAlignment"	"center"	
		"labelText"		"#TF_WeaponSwitchHint"	
		"font"			"TallTextSmall"
		"fgcolor"		"TanLight"
	}	
	"HintLabelS"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"HintLabelS"
		"xpos"			"1"
		"ypos"			"-10"
		"zpos"			"1"
		"wide"			"150"
		"tall"			"35"
		"visible"		"1"
		"enabled"		"1"
		"textAlignment"	"center"	
		"labelText"		"#TF_WeaponSwitchHint"	
		"font"			"TallTextSmall"
		"fgcolor"		"Black"
	}
	"PanelBG"
	{
		"ControlName"	"CTFImagePanel"
		"fieldName"		"PanelBG"
		"xpos"			"0"
		"ypos"			"5"	[$WIN32]
		"zpos"			"1"
		"wide"			"200"
		"tall"			"100"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/weapon_bucket_black"
		"scaleImage"	"1"	
		"teambg_0"		"../hud/weapon_bucket_select_null"
		"teambg_1"		"../hud/weapon_bucket_select_black"
		"teambg_2"		"../hud/weapon_bucket_select_red"
		"teambg_3"		"../hud/weapon_bucket_select_blue"	
		"teambg_4"		"../hud/weapon_bucket_select_green"
		"teambg_5"		"../hud/weapon_bucket_select_yellow"
	}	
	"HudColoredBG"
	{
		"ControlName"	"CTFImageColoredPanel"
		"fieldName"		"HudColoredBG"
		"xpos"			"0"
		"ypos"			"5"
		"zpos"			"0"
		"wide"			"200"
		"tall"			"100"
		"visible"		"1"
		"enabled"		"1"
		"alpha"			"230"
		"image"			"../hud/weapon_bucket_select_custom"
		"scaleImage"	"1"	
	}	
	"ArrowImage"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"ArrowImage"
		"xpos"			"22"
		"ypos"			"12"
		"zpos"			"2"
		"wide"			"80"
		"tall"			"80"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/arrows"
		"alpha"			"220"
		"scaleImage"	"1"	
	}	
	"WeaponFrom"
	{
		"ControlName"	"CItemModelPanel"
		"fieldName"		"WeaponFrom"
		"xpos"			"5"
		"ypos"			"8"
		"zpos"			"4"
		"wide"			"70"
		"tall"			"50"
		"bgcolor_override"		"0 0 0 0"
		"PaintBackgroundType"	"2"
		"paintborder"	"1"
		"name_only"		"1"
		"model_ypos"	"5"
		"standard_text_color"	"1"

		"itemmodelpanel"
		{
			"use_item_rendertarget" "0"
			"allow_rot"				"0"
		}
	}
	"WeaponTo"
	{
		"ControlName"	"CItemModelPanel"
		"fieldName"		"WeaponTo"
		"xpos"			"55"
		"ypos"			"28"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"65"
		"bgcolor_override"		"0 0 0 0"
		"PaintBackgroundType"	"2"
		"paintborder"	"1"
		"name_only"		"1"
		"model_ypos"	"5"
		"standard_text_color"	"1"

		"itemmodelpanel"
		{
			"use_item_rendertarget" "0"
			"allow_rot"				"0"
		}
	}
}