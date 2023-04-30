defmodule HNSWLib.Index.Test do
  use ExUnit.Case
  doctest HNSWLib.Index

  test "HNSWLib.Index.new/3 with L2-space" do
    space = :l2
    dim = 8
    max_elements = 200
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    dim = 12
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    space = :cosine
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    space = :ip
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim
  end

  test "HNSWLib.Index.new/3 with cosine-space" do
    space = :cosine
    dim = 8
    max_elements = 200
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    dim = 12
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim
  end

  test "HNSWLib.Index.new/3 with inner-product space" do
    space = :ip
    dim = 8
    max_elements = 200
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    dim = 12
    {:ok, index} = HNSWLib.Index.new(space, dim, max_elements)

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim
  end

  test "HNSWLib.Index.new/3 with non-default keyword parameters" do
    space = :ip
    dim = 8
    max_elements = 200

    m = 200
    ef_construction = 400
    random_seed = 42
    allow_replace_deleted = true

    {:ok, index} =
      HNSWLib.Index.new(space, dim, max_elements,
        m: m,
        ef_construction: ef_construction,
        random_seed: random_seed,
        allow_replace_deleted: allow_replace_deleted
      )

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim

    dim = 12

    {:ok, index} =
      HNSWLib.Index.new(space, dim, max_elements,
        m: m,
        ef_construction: ef_construction,
        random_seed: random_seed,
        allow_replace_deleted: allow_replace_deleted
      )

    assert is_reference(index.reference)
    assert space == index.space
    assert dim == index.dim
  end

  test "HNSWLib.Index.new/3 with invalid keyword parameter m" do
    space = :ip
    dim = 8
    max_elements = 200

    m = -1

    assert {:error,
            "expect keyword parameter `:m` to be a non-negative integer, got `#{inspect(m)}`"} ==
             HNSWLib.Index.new(space, dim, max_elements, m: m)
  end

  test "HNSWLib.Index.new/3 with invalid keyword parameter ef_construction" do
    space = :ip
    dim = 8
    max_elements = 200

    ef_construction = -1

    assert {:error,
            "expect keyword parameter `:ef_construction` to be a non-negative integer, got `#{inspect(ef_construction)}`"} ==
             HNSWLib.Index.new(space, dim, max_elements, ef_construction: ef_construction)
  end

  test "HNSWLib.Index.new/3 with invalid keyword parameter random_seed" do
    space = :ip
    dim = 8
    max_elements = 200

    random_seed = -1

    assert {:error,
            "expect keyword parameter `:random_seed` to be a non-negative integer, got `#{inspect(random_seed)}`"} ==
             HNSWLib.Index.new(space, dim, max_elements, random_seed: random_seed)
  end

  test "HNSWLib.Index.new/3 with invalid keyword parameter allow_replace_deleted" do
    space = :ip
    dim = 8
    max_elements = 200

    allow_replace_deleted = -1

    assert {:error,
            "expect keyword parameter `:allow_replace_deleted` to be a boolean, got `#{inspect(allow_replace_deleted)}`"} ==
             HNSWLib.Index.new(space, dim, max_elements,
               allow_replace_deleted: allow_replace_deleted
             )
  end
end
