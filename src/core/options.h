// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.
#pragma once

#include <map>
#include "../include/librealsense/rs2.h"
#include "extension.h"
#include "types.h"

namespace librealsense
{
    struct option_range
    {
        float min;
        float max;
        float step;
        float def;
    };

    class option //TODO: Ziv, : public recordable<option>
    {
    public:
        virtual void set(float value) = 0;
        virtual float query() const = 0;
        virtual option_range get_range() const = 0;
        virtual bool is_enabled() const = 0;
        virtual bool is_read_only() const { return false; }

        virtual const char* get_description() const = 0;
        virtual const char* get_value_description(float) const { return nullptr; }

        virtual ~option() = default;
    };

    class options_interface//TODO?: Ziv, public recordable<options_interface>
    {
    public:
        virtual option& get_option(rs2_option id) = 0;
        virtual const option& get_option(rs2_option id) const = 0;
        virtual bool supports_option(rs2_option id) const = 0;

        virtual ~options_interface() = default;
    };

    MAP_EXTENSION(RS2_EXTENSION_OPTIONS, librealsense::options_interface);

    class options_container : public virtual options_interface
    {
    public:
        bool supports_option(rs2_option id) const override
        {
            auto it = _options.find(id);
            if (it == _options.end()) return false;
            return it->second->is_enabled();
        }

        option& get_option(rs2_option id) override
        {
            auto it = _options.find(id);
            if (it == _options.end())
            {
                throw invalid_value_exception(to_string()
                    << "Device does not support option "
                    << rs2_option_to_string(id) << "!");
            }
            return *it->second;
        }

        const option& get_option(rs2_option id) const override
        {
            auto it = _options.find(id);
            if (it == _options.end())
            {
                throw invalid_value_exception(to_string()
                    << "Device does not support option "
                    << rs2_option_to_string(id) << "!");
            }
            return *it->second;
        }

        void register_option(rs2_option id, std::shared_ptr<option> option)
        {
            _options[id] = option;
        }

    private:
        std::map<rs2_option, std::shared_ptr<option>> _options;
    };
}
