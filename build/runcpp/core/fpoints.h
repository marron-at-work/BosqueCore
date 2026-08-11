#pragma once

#include "../common.h"

#include "bsqtype.h"
#include "bools.h"

namespace ᐸRuntimeᐳ 
{
    class XFloat
    {
    public:
        double value;

        template <typename T>
        static XBool isSafeConvertInto(XFloat f)
        {
            if(!std::isfinite(f.value)) {
                return XBool::from(false);
            }

            return XBool::from((static_cast<double>(T::MIN_VALUE) <= f.value) & (f.value <= static_cast<double>(T::MAX_VALUE)));
        }

        template <typename T>
        static T doSafeConvertInto(XFloat f)
        {
            return T{static_cast<typename T::value_type>(f.value)};
        }

        // Overloaded operators on Float
        XFloat operator+() const
        {
            return XFloat{this->value};
        }
        XFloat operator-() const
        {
            return XFloat{-this->value};
        }

        friend XFloat operator+(XFloat lhs, XFloat rhs)
        {
            return XFloat{lhs.value + rhs.value};
        }
        friend XFloat operator-(XFloat lhs, XFloat rhs)
        {
            return XFloat{lhs.value - rhs.value};
        }
        friend XFloat operator/(XFloat lhs, XFloat rhs)
        {
           return XFloat{lhs.value / rhs.value};
        }
        friend XFloat operator*(XFloat lhs, XFloat rhs)
        {
            return XFloat{lhs.value * rhs.value};
        }

        friend XBool operator<(const XFloat &lhs, const XFloat &rhs) { return XBool::from(lhs.value < rhs.value); }
        friend XBool operator==(const XFloat &lhs, const XFloat &rhs) { return XBool::from(lhs.value == rhs.value); }
        friend XBool operator>(const XFloat &lhs, const XFloat &rhs) { return XBool::from(rhs.value < lhs.value); }
        friend XBool operator!=(const XFloat &lhs, const XFloat &rhs) { return XBool::from(!(lhs.value == rhs.value)); }
        friend XBool operator<=(const XFloat &lhs, const XFloat &rhs) { return XBool::from(!(lhs.value > rhs.value)); }
        friend XBool operator>=(const XFloat &lhs, const XFloat &rhs) { return XBool::from(!(lhs.value < rhs.value)); }
    };

    inline constexpr TypeInfo g_typeinfo_Float = {
        WELL_KNOWN_TYPE_ID_FLOAT,
        sizeof(XFloat),
        byteSizeToSlotCount(sizeof(XFloat)),
        LayoutTag::Value,
        BSQ_PTR_MASK_LEAF,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        "Float",
        true
    };

    static_assert(sizeof(XFloat) == sizeof(double), "Float size incorrect");
}
