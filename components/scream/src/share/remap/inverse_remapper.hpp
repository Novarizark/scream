#ifndef SCREAM_INVERSE_REMAPPER_HPP
#define SCREAM_INVERSE_REMAPPER_HPP

#include "share/remap/abstract_remapper.hpp"

namespace scream
{

// Performs remap by chaining two remap strategies
template<typename ScalarType, typename DeviceType>
class InverseRemapper : public AbstractRemapper<ScalarType,DeviceType>
{
public:
  using base_type       = AbstractRemapper<ScalarType,DeviceType>;
  using field_type      = typename base_type::field_type;
  using identifier_type = typename base_type::identifier_type;
  using layout_type     = typename base_type::layout_type;

  InverseRemapper (std::shared_ptr<base_type> remapper) :
    base_type(remapper->get_tgt_grid(),remapper->get_src_grid())
  {
    error::runtime_check(static_cast<bool>(remapper), "Error! Null pointer for inner remapper.\n");

    m_remapper = remapper;
  }

  ~InverseRemapper () = default;

  FieldLayout create_src_layout (const FieldLayout& tgt_layout) const override {
    return m_remapper->create_tgt_layout(tgt_layout);
  }
  FieldLayout create_tgt_layout (const FieldLayout& src_layout) const override {
    return m_remapper->create_src_layout(src_layout);
  }

protected:

  const identifier_type& do_get_src_field_id (const int ifield) const override {
    return m_remapper->get_tgt_field_id(ifield);
  }
  const identifier_type& do_get_tgt_field_id (const int ifield) const override {
    return m_remapper->get_src_field_id(ifield);
  }
  const field_type& do_get_src_field (const int ifield) const override {
    return m_remapper->get_src_field(ifield);
  }
  const field_type& do_get_tgt_field (const int ifield) const override {
    return m_remapper->get_src_field(ifield);
  }

  void do_remap_fwd () const override {
    m_remapper->remap(false);
  }
  void do_remap_bwd () const override {
    m_remapper->remap(true);
  }

  void do_registration_begins () override {
    m_remapper->set_num_fields(this->m_num_fields);
    m_remapper->registration_begins();
  }
  void do_register_field (const identifier_type& src, const identifier_type& tgt) override {
    m_remapper->register_field(tgt,src);
  }
  void do_bind_field (const int ifield, const field_type& src, const field_type& tgt) override {
    m_remapper->bind_field(ifield,tgt,src);
  }
  void do_registration_complete () override {
    m_remapper->registration_complete();
  }

  std::shared_ptr<base_type>  m_remapper;
};

} // namespace scream

#endif // SCREAM_INVERSE_REMAPPER_HPP
